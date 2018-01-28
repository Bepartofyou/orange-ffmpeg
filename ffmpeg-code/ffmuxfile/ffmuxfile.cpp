#include <stdlib.h>
#include <stdio.h>
#include "ffmuxfile.h"

CffmuxFile::CffmuxFile(){
	m_sVideoOst = { 0 };
	m_sAudioOst = { 0 };
	m_pOpt = NULL;
	m_pOutFmt = NULL;
	m_pOutFmtCtx = NULL;
	
	m_bHaveVideo = false;
	m_bHaveAudio = false;
	m_bReadVideo = false;
	m_bReadAudio = false;

	m_bH264Video = false;
}

CffmuxFile::~CffmuxFile() {};

void CffmuxFile::SetBase(const char* audiofile, const char* videofile, const char* outfile, bool bvideoH264) {

	m_strAudioFile = audiofile;
	m_strVideoFile = videofile;
	m_strOutFile = outfile;

	m_bH264Video = bvideoH264;
}

int CffmuxFile::Create() {

	int ret;
	/* Initialize libavcodec, and register all codecs and formats. */
	av_register_all();

	/* allocate the output media context */
	avformat_alloc_output_context2(&m_pOutFmtCtx, NULL, NULL, m_strOutFile.c_str());
	if (!m_pOutFmtCtx) {
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&m_pOutFmtCtx, NULL, "mp4", m_strOutFile.c_str());
	}
	if (!m_pOutFmtCtx) {
		Destory();
		return -1;
	}

	m_pOutFmt = m_pOutFmtCtx->oformat;

	if (m_strAudioFile.c_str() != NULL){

		addStream(m_pOutFmtCtx, &m_sAudioOst, m_strAudioFile.c_str(), true);
		m_sAudioOst.bsfc = av_bitstream_filter_init("aac_adtstoasc");

		m_bHaveAudio = true;
		m_bReadAudio = true;
	}
	if (m_strVideoFile.c_str() != NULL){

		addStream(m_pOutFmtCtx, &m_sVideoOst, m_strVideoFile.c_str(), false);
		m_sVideoOst.bsfc = av_bitstream_filter_init("h264_mp4toannexb");

		m_bHaveVideo = true;
		m_bReadVideo = true;
	}

	/* Now that all the parameters are set, we can open the audio and
	* video codecs and allocate the necessary encode buffers. */
	if (m_bHaveVideo)
		initVideo();

	if (m_bHaveAudio)
		initAudio();

	av_dump_format(m_pOutFmtCtx, 0, m_strOutFile.c_str(), 1);

	printf("audio timebase, num:%d,den:%d,video timebase, nem:%d,den:%d\n", m_pOutFmtCtx->streams[m_sAudioOst.output_index]->time_base.num,
		m_pOutFmtCtx->streams[m_sAudioOst.output_index]->time_base.den, m_pOutFmtCtx->streams[m_sVideoOst.output_index]->time_base.num,
		m_pOutFmtCtx->streams[m_sVideoOst.output_index]->time_base.den);

	printf("audio timebase, num:%d,den:%d,video timebase, nem:%d,den:%d\n", m_sAudioOst.input_st->time_base.num,
		m_sAudioOst.input_st->time_base.den, m_sVideoOst.input_st->time_base.num,
		m_sVideoOst.input_st->time_base.den);

	/* open the output file, if needed */
	if (!(m_pOutFmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&m_pOutFmtCtx->pb, m_strOutFile.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			Destory();
			return -1;
		}
	}

	/* Write the stream header, if any. */
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "com.apple.quicktime.commen", "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "title",						 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "artist",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "album_artist",				 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "composer",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "album",						 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "date",						 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "encoding_tool",				 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "comment",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "genre",						 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "copyright",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "grouping",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "lyrics",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "description",				 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "synopsis",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "show",						 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "episode_id",				 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "network",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "episode_sort",				 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "season_number",				 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "media_type",				 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "hd_video",					 "Bepartofyou", 0);
	ret = av_dict_set(&m_pOutFmtCtx->metadata, "gapless_playback",			 "Bepartofyou", 0);

	if (!strcmp(m_pOutFmt->name, "mp4")) {
		av_dict_set_int(&m_pOpt, "movflags", FF_MOV_FLAG_FASTSTART, 0);
	}
}

void CffmuxFile::Destory() {

	if (m_pOpt)
		av_dict_free(&m_pOpt);

	/* Close each codec. */
	if (m_bHaveVideo)
		closeStream(&m_sVideoOst);
	if (m_bHaveAudio)
		closeStream(&m_sAudioOst);

	if (!(m_pOutFmt->flags & AVFMT_NOFILE))
		/* Close the output file. */
		avio_closep(&m_pOutFmtCtx->pb);

	/* free the stream */
	avformat_free_context(m_pOutFmtCtx);
}

/* Add an output stream. */
void CffmuxFile::addStream(AVFormatContext *ofmt_ctx, OutputStream  *ost, const char* inputfilename, bool baudio)
{
	int ret;
	if ((ret = avformat_open_input(&ost->input_fmtctx, inputfilename, 0, 0)) < 0) {
		fprintf(stderr, "Could not open input file '%s'", inputfilename);
	}

	if ((ret = avformat_find_stream_info(ost->input_fmtctx, 0)) < 0) {
		fprintf(stderr, "Failed to retrieve input stream information");
	}

	av_dump_format(ost->input_fmtctx, 0, inputfilename, 0);

	enum AVMediaType av_media_type = baudio ? AVMEDIA_TYPE_AUDIO : AVMEDIA_TYPE_VIDEO;

	for (unsigned int i = 0; i < ost->input_fmtctx->nb_streams; i++) {

		if (ost->input_fmtctx->streams[i]->codec->codec_type == av_media_type) {

			AVStream *in_stream = ost->input_fmtctx->streams[i];
			AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
			if (!out_stream)
				fprintf(stderr, "Failed allocating output stream\n");

			ost->input_index = i;
			ost->output_index = out_stream->index;
			ost->input_st = ost->input_fmtctx->streams[i];

			ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
			if (ret < 0)
				fprintf(stderr, "Failed to copy context from input to output stream codec context\n");

			//add AVStream timebase
			//ofmt_ctx->streams[out_stream->index]->time_base = in_stream->time_base;
			if (baudio)
				ofmt_ctx->streams[out_stream->index]->time_base = AVRational{ 1, in_stream->codec->sample_rate };
			else {
				//ret = av_dict_set(&out_stream->metadata, "rotate", "90", 0); //ÉèÖÃÐý×ª½Ç¶È
				ofmt_ctx->streams[out_stream->index]->time_base = in_stream->time_base;
			}

			out_stream->codec->codec_tag = 0;
			if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER){

				out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
				//out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
			}
		}
	}
}

/* Close input stream. */
void CffmuxFile::closeStream(OutputStream  *ost)
{
	av_bitstream_filter_close(ost->bsfc);
	avformat_close_input(&ost->input_fmtctx);
}

void CffmuxFile::initVideo()
{
	while (av_read_frame(m_sVideoOst.input_fmtctx, &m_sVideoOst.pkt) >= 0)
	{
		if (m_sVideoOst.pkt.stream_index == m_sVideoOst.input_index) {

			if (!m_bH264Video) {
				if (m_sVideoOst.pkt.pts != AV_NOPTS_VALUE)
					m_sVideoOst.offset = m_sVideoOst.pkt.pts;
				else
					m_sVideoOst.offset = 0;

				int64_t diff = m_sVideoOst.pkt.pts - m_sVideoOst.pkt.dts;
				m_sVideoOst.next_pts = 0;
				m_sVideoOst.pkt.pts = m_sVideoOst.next_pts;
				m_sVideoOst.pkt.dts = m_sVideoOst.pkt.pts - diff;
				printf("video init offset:%ld\n", m_sVideoOst.offset);
			}
			else {
				m_sVideoOst.next_pts = 0;
				m_sVideoOst.frames_count += 1;

				m_sVideoOst.pkt.pts = m_sVideoOst.next_pts;
				printf("video init offset:%ld\n", m_sVideoOst.offset);
			}
			
			return;
		}
		else
			av_free_packet(&m_sVideoOst.pkt);   //free the unexpected pkt
	}

	m_sVideoOst.next_pts = XX_MAX_PTS;
}

void CffmuxFile::initAudio()
{
	while (av_read_frame(m_sAudioOst.input_fmtctx, &m_sAudioOst.pkt) >= 0)
	{
		if (m_sAudioOst.pkt.stream_index == m_sAudioOst.input_index) {

			m_sAudioOst.next_pts = 0;
			m_sAudioOst.samples_count += m_sAudioOst.input_st->codec->frame_size;

			m_sAudioOst.pkt.pts = m_sAudioOst.next_pts;
			printf("audio init offset:%ld\n", m_sAudioOst.offset);

			return;
		}
		else
			av_free_packet(&m_sAudioOst.pkt);   //free the unexpected pkt
	}

	m_sAudioOst.next_pts = XX_MAX_PTS;
}

int CffmuxFile::writeVideoFrame()
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(&m_sVideoOst.pkt, m_sVideoOst.input_st->time_base, m_pOutFmtCtx->streams[m_sVideoOst.output_index]->time_base);
	m_sVideoOst.pkt.stream_index = m_sVideoOst.output_index;

	if (m_sVideoOst.pkt.pts <= m_sVideoOst.last_pts)
		m_sVideoOst.pkt.pts = m_sVideoOst.last_pts + 1;
	// only used without B frames
	//m_sVideoOst.pkt.dts = m_sVideoOst.pkt.pts;

	printf("video pts:%ld,dts:%ld\n", m_sVideoOst.pkt.pts, m_sVideoOst.pkt.dts);
	/* Write the compressed frame to the media file. */
	logPacket(m_pOutFmtCtx, &m_sVideoOst.pkt);

	m_sVideoOst.last_pts = m_sVideoOst.pkt.pts;

	av_interleaved_write_frame(m_pOutFmtCtx, &m_sVideoOst.pkt);

	while (av_read_frame(m_sVideoOst.input_fmtctx, &m_sVideoOst.pkt) >= 0)
	{
		if (m_sVideoOst.pkt.stream_index == m_sVideoOst.input_index) {

			if (!m_bH264Video) {

				int64_t diff = m_sVideoOst.pkt.pts - m_sVideoOst.pkt.dts;

				if (m_sVideoOst.pkt.pts != AV_NOPTS_VALUE)
				{
					m_sVideoOst.next_pts = m_sVideoOst.pkt.pts - m_sVideoOst.offset;
					m_sVideoOst.pkt.pts = m_sVideoOst.next_pts;
					m_sVideoOst.pkt.dts = m_sVideoOst.pkt.pts - diff;
				}
				else
					m_sVideoOst.next_pts = 0;
			}
			else {

				m_sVideoOst.next_pts = av_rescale_q(m_sVideoOst.frames_count, 
					AVRational{ m_sVideoOst.input_st->r_frame_rate.den, m_sVideoOst.input_st->r_frame_rate.num}, m_sVideoOst.input_st->time_base);
				m_sVideoOst.pkt.pts = m_sVideoOst.next_pts;
				// only used without B frames
				m_sVideoOst.pkt.dts = m_sVideoOst.pkt.pts;
				m_sVideoOst.frames_count += 1;
			}

			return false;
		}
		else
			av_free_packet(&m_sVideoOst.pkt);   //free the unexpected pkt
	}

	return true;
}

int CffmuxFile::writeAudioFrame()
{

	av_bitstream_filter_filter(m_sAudioOst.bsfc, m_pOutFmtCtx->streams[m_sAudioOst.output_index]->codec,
		NULL, &(m_sAudioOst.pkt.data), &(m_sAudioOst.pkt.size), m_sAudioOst.pkt.data, m_sAudioOst.pkt.size, 0);

	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(&m_sAudioOst.pkt, m_sAudioOst.input_st->time_base, m_pOutFmtCtx->streams[m_sAudioOst.output_index]->time_base);
	m_sAudioOst.pkt.stream_index = m_sAudioOst.output_index;

	if (m_sAudioOst.pkt.pts <= m_sAudioOst.last_pts)
		m_sAudioOst.pkt.pts = m_sAudioOst.last_pts + 1;
	m_sAudioOst.pkt.dts = m_sAudioOst.pkt.pts;

	printf("audio pts:%ld,dts:%ld\n", m_sAudioOst.pkt.pts, m_sAudioOst.pkt.dts);
	/* Write the compressed frame to the media file. */
	logPacket(m_pOutFmtCtx, &m_sAudioOst.pkt);

	m_sAudioOst.last_pts = m_sAudioOst.pkt.pts;

	av_interleaved_write_frame(m_pOutFmtCtx, &(m_sAudioOst.pkt));

	while (av_read_frame(m_sAudioOst.input_fmtctx, &m_sAudioOst.pkt) >= 0)
	{
		if (m_sAudioOst.pkt.stream_index == m_sAudioOst.input_index) {

			//nb_samples
			m_sAudioOst.next_pts = av_rescale_q(m_sAudioOst.samples_count, AVRational{ 1, m_sAudioOst.input_st->codec->sample_rate }, m_sAudioOst.input_st->time_base);
			m_sAudioOst.pkt.pts = m_sAudioOst.next_pts;
			m_sAudioOst.samples_count += m_sAudioOst.input_st->codec->frame_size;

			return false;
		}
		else
			av_free_packet(&m_sAudioOst.pkt);   //free the unexpected pkt
	}

	return true;
}


void CffmuxFile::logPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
	AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

	printf("duration:%f\n", (float)pkt->pts*(float)time_base->num / (float)time_base->den);

	return;
}

int CffmuxFile::Muxer() {
	int ret;
	ret = avformat_write_header(m_pOutFmtCtx, &m_pOpt);
	//ret = avformat_write_header(out_fmtctx, NULL);
	if (ret < 0) {
		Destory();
		return -1;
	}

	while (m_bReadVideo || m_bReadAudio) {
		/* select the stream to encode */

		if (m_bReadVideo &&
			(!m_bReadAudio || av_compare_ts(m_sVideoOst.next_pts, m_sVideoOst.input_st->time_base,
				m_sAudioOst.next_pts, m_sAudioOst.input_st->time_base) <= 0)) {
			m_bReadVideo = !writeVideoFrame();
		}
		else {
			m_bReadAudio = !writeAudioFrame();
		}
	}

	/* Write the trailer, if any. The trailer must be written before you
	* close the CodecContexts open when you wrote the header; otherwise
	* av_write_trailer() may try to use memory that was freed on
	* av_codec_close(). */

	av_write_trailer(m_pOutFmtCtx);

	return 0;
}