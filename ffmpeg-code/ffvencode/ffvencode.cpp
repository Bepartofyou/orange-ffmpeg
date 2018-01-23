#include <string>

#include "ffvencode.h"

CffVencode::CffVencode() :m_uFrameCount(0) {}

CffVencode::~CffVencode() {}

void CffVencode::SetBase(int width, int height, int gopsize, int64_t bitrate, AVRational timebase, AVRational framerate, enum AVPixelFormat pixfmt) {
	m_iWidth = width;
	m_iHeight = height;
	m_iGopSize = gopsize;
	m_iBitRate = bitrate;
	m_sTimeBase = timebase;
	m_sFrameRate = framerate;
	m_ePixFmt = pixfmt;
}

int CffVencode::Create(const char* codecname) {
	int ret;

	avcodec_register_all();

	/* find the codecname encoder */
	m_pCodec = avcodec_find_encoder_by_name("libx264");
	if (!m_pCodec) {
		fprintf(stderr, "Codec '%s' not found\n", codecname);
		return -1;
	}

	m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
	if (!m_pCodecCtx) {
		fprintf(stderr, "Could not allocate video codec context\n");
		return -1;
	}

	m_pPkt = av_packet_alloc();
	if (!m_pPkt)
		return -1;

	/* put sample parameters */
	m_pCodecCtx->bit_rate = m_iBitRate;
	/* resolution must be a multiple of two */
	m_pCodecCtx->width = m_iWidth;
	m_pCodecCtx->height = m_iHeight;
	/* frames per second */
	m_pCodecCtx->time_base = m_sTimeBase;
	m_pCodecCtx->framerate = m_sFrameRate;

	/* emit one intra frame every ten frames
	* check frame pict_type before passing frame
	* to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
	* then gop_size is ignored and the output of encoder
	* will always be I frame irrespective to gop_size
	*/
	m_pCodecCtx->gop_size = m_iGopSize;
	m_pCodecCtx->max_b_frames = 0;
	m_pCodecCtx->pix_fmt = m_ePixFmt;

	if (m_pCodec->id == AV_CODEC_ID_H264)
		av_opt_set(m_pCodecCtx->priv_data, "preset", "slow", 0);

	/* open it */
	ret = avcodec_open2(m_pCodecCtx, m_pCodec, NULL);
	if (ret < 0) {
		//fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
		return -1;
	}

	m_pFrame = av_frame_alloc();
	if (!m_pFrame) {
		fprintf(stderr, "Could not allocate video frame\n");
		return -1;
	}
	m_pFrame->format = m_pCodecCtx->pix_fmt;
	m_pFrame->width = m_pCodecCtx->width;
	m_pFrame->height = m_pCodecCtx->height;

	/*
	* This align will affect frame->linesize,
	* should use lineszie to copy yuv data to frame data
	* without caring about the number of aling parameter
	*/
	//ret = av_frame_get_buffer(frame, 32); 
	ret = av_frame_get_buffer(m_pFrame, 0); // ffmpeg interface suggest
	if (ret < 0) {
		fprintf(stderr, "Could not allocate the video frame data\n");
		return -1;
	}

	return 0;
}

void CffVencode::Destory() {

	avcodec_free_context(&m_pCodecCtx);
	av_frame_free(&m_pFrame);
	av_packet_free(&m_pPkt);
}

int CffVencode::Encode(uint8_t* yuv_data, int yuv_size, uint8_t* h264_data, int& h264_size, std::vector<int>& vsize) {

	if (!h264_data)
		return -1;
	//if (yuv_size != m_iWidth * m_iHeight * av_get_bits_per_pixel(av_pix_fmt_desc_get(m_ePixFmt)) / 8)
	//	return -1;

	int ret, index;

	if (yuv_data) {

		/* make sure the frame data is writable */
		ret = av_frame_make_writable(m_pFrame);
		if (ret < 0)
			return -1;

		/* Y */
		for (index = 0; index < m_pCodecCtx->height; index++) {
			memcpy(&m_pFrame->data[0][index * m_pFrame->linesize[0]], yuv_data + index*m_pCodecCtx->width, m_pCodecCtx->width);
		}

		/* Cb and Cr */
		for (index = 0; index < m_pCodecCtx->height / 2; index++) {
			memcpy(&m_pFrame->data[1][index * m_pFrame->linesize[1]], 
				yuv_data + m_pCodecCtx->width*m_pCodecCtx->height + index*m_pCodecCtx->width / 2, m_pCodecCtx->width / 2);
			memcpy(&m_pFrame->data[2][index * m_pFrame->linesize[2]], 
				yuv_data + m_pCodecCtx->width*m_pCodecCtx->height * 5 / 4 + index*m_pCodecCtx->width / 2, m_pCodecCtx->width / 2);
		}

		m_pFrame->pts = m_uFrameCount* (m_sTimeBase.den * m_sFrameRate.den) / (m_sTimeBase.num * m_sFrameRate.num);
		//m_pFrame->pts = m_uFrameCount;
		m_uFrameCount++;
	
		/* send the frame to the encoder */
		if (m_pFrame)
			printf("Send frame %lld\n", m_pFrame->pts);

		ret = avcodec_send_frame(m_pCodecCtx, m_pFrame);
		if (ret < 0) {
			fprintf(stderr, "Error sending a frame for encoding\n");
			return -1;
		}
	}
	else{
		ret = avcodec_send_frame(m_pCodecCtx, NULL);
		if (ret < 0) {
			fprintf(stderr, "Error sending a frame for encoding\n");
			return -1;
		}
	}

	int length = 0;
	while (ret >= 0) {
		ret = avcodec_receive_packet(m_pCodecCtx, m_pPkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			//return -1;
			break;
		else if (ret < 0) {
			fprintf(stderr, "Error during encoding\n");
			return -1;
		}

		//printf("Write packet %lld (size=%5d)\n", m_pPkt->pts, m_pPkt->size);

		memcpy(h264_data + length, m_pPkt->data, m_pPkt->size);
		length += m_pPkt->size;
		vsize.push_back(m_pPkt->size);

		av_packet_unref(m_pPkt);
	}

	h264_size = length;

	return 0;
}
