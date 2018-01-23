#ifndef _FF_VENCODE_H_
#define _FF_VENCODE_H_

#include <vector>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

#ifdef __cplusplus
}
#endif // __cplusplus

class CffVencode
{
public:
	CffVencode();
	~CffVencode();

private:
	AVCodec *m_pCodec;
	AVCodecContext *m_pCodecCtx;
	enum AVCodecID m_eCodecID;

	AVFrame *m_pFrame;
	AVPacket *m_pPkt;

	int m_iWidth;
	int m_iHeight;
	int m_iGopSize;
	int64_t m_iBitRate;
	AVRational m_sTimeBase;
	AVRational m_sFrameRate;
	enum AVPixelFormat m_ePixFmt;

	uint32_t m_uFrameCount;
public:
	void SetBase(int width, int height, int gopsize, int64_t bitrate, AVRational timebase, AVRational framerate, enum AVPixelFormat pixfmt);
	int Create(const char* codecname);
	int Encode(uint8_t* yuv_data, int yuv_size, uint8_t* h264_data, int& h264_size, std::vector<int>& vsize);
	void Destory();
};

#endif // _FF_VENCODE_H_