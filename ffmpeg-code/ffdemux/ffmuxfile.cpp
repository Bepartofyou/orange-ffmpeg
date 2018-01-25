#include <stdlib.h>
#include <stdio.h>
#include "ffmuxfile.h"
#include "mpeg4-aac.h"

#include <sstream>

CffmuxFile::CffmuxFile() { 
	m_iVideoIndex = -1;
	m_iAudioIndex = -1;
	m_pInFmtCtx = NULL; 
	m_pAf = NULL;
	m_pVf = NULL;
}

CffmuxFile::~CffmuxFile() {}

int CffmuxFile::Create() {

	int ret;

	m_pAf = fopen(m_strAoutFile.c_str(), "wb");
	if (!m_pAf)
		return -1;
	m_pVf = fopen(m_strVoutFile.c_str(), "wb");
	if (!m_pVf)
		return -1;

	/* Initialize libavcodec, and register all codecs and formats. */
	av_register_all();

	if ((ret = avformat_open_input(&m_pInFmtCtx, m_strInFile.c_str(), 0, 0)) < 0) {
		fprintf(stderr, "Could not open input file '%s'", m_strInFile.c_str());
		Destory();
		return -1;
	}

	if ((ret = avformat_find_stream_info(m_pInFmtCtx, 0)) < 0) {
		fprintf(stderr, "Failed to retrieve input stream information");
		Destory();
		return -1;
	}

	av_dump_format(m_pInFmtCtx, 0, m_strInFile.c_str(), 0);

	for (unsigned int i = 0; i < m_pInFmtCtx->nb_streams; i++) {
		if (m_pInFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			m_iVideoIndex = i;
		else if (m_pInFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			m_iAudioIndex = i;
	}

	if (m_iVideoIndex == -1 || m_iAudioIndex == -1) {
		fprintf(stderr, "don't have audio and video synchronous");
		Destory();
		return -1;
	}

	m_pAudioBsfc = av_bitstream_filter_init("aac_adtstoasc");
	m_pVideoBsfc = av_bitstream_filter_init("h264_mp4toannexb");

	return 0;
}

void CffmuxFile::Destory() {

	fclose(m_pAf);
	fclose(m_pVf);

	av_bitstream_filter_close(m_pAudioBsfc);
	av_bitstream_filter_close(m_pVideoBsfc);
	avformat_close_input(&m_pInFmtCtx);
}

int CffmuxFile::Demux() {

	static bool offsetflag = false;
	static int offset = 0;
	
	while (av_read_frame(m_pInFmtCtx, &m_sPkt) >= 0)
	{
		if (m_sPkt.stream_index == m_iVideoIndex){

			if (!offsetflag) {
				int pts = m_sPkt.pts * 1000 * m_pInFmtCtx->streams[m_iVideoIndex]->time_base.num / m_pInFmtCtx->streams[m_iVideoIndex]->time_base.den;
				if (pts < 0)
					offset = -pts;
				offsetflag = true;
			}

			av_bitstream_filter_filter(m_pVideoBsfc, m_pInFmtCtx->streams[m_iVideoIndex]->codec,
				NULL, &(m_sPkt.data), &(m_sPkt.size), m_sPkt.data, m_sPkt.size, 0);

			int pts = m_sPkt.pts * 1000 * m_pInFmtCtx->streams[m_iVideoIndex]->time_base.num / m_pInFmtCtx->streams[m_iVideoIndex]->time_base.den
				+ offset;

			fwrite(m_sPkt.data, 1, m_sPkt.size, m_pVf);
		}
		else if (m_sPkt.stream_index == m_iAudioIndex) {

			if (!offsetflag) {
				int pts = m_sPkt.pts * 1000 * m_pInFmtCtx->streams[m_iAudioIndex]->time_base.num / m_pInFmtCtx->streams[m_iAudioIndex]->time_base.den;
				if (pts< 0)
					offset = -pts;
				offsetflag = true;
			}

			//av_bitstream_filter_filter(m_pAudioBsfc, m_pInFmtCtx->streams[m_iAudioIndex]->codec,
			//	NULL, &(m_sPkt.data), &(m_sPkt.size), m_sPkt.data, m_sPkt.size, 0);

			mpeg4_aac_t adts = { 0 };
			adts.channel_configuration = m_pInFmtCtx->streams[m_iAudioIndex]->codec->channels;
			int sample = m_pInFmtCtx->streams[m_iAudioIndex]->codec->frame_size;
			adts.sampling_frequency_index = mpeg4_aac_audio_frequency_from(m_pInFmtCtx->streams[m_iAudioIndex]->codec->sample_rate / (sample / 1024));
			adts.profile = m_pInFmtCtx->streams[m_iAudioIndex]->codec->profile;

			mpeg4_aac_adts_save(&adts, m_sPkt.size, m_uADTS, 7);

			int pts = m_sPkt.pts * 1000 * m_pInFmtCtx->streams[m_iAudioIndex]->time_base.num / m_pInFmtCtx->streams[m_iAudioIndex]->time_base.den
				+ offset;

			fwrite(m_uADTS, 1, 7, m_pAf);
			fwrite(m_sPkt.data, 1, m_sPkt.size, m_pAf);
		}
		av_free_packet(&m_sPkt);
	}

	return 0;
}
