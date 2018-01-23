#ifndef _FF_SWSCALE_H_
#define _FF_SWSCALE_H_

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif // __cplusplus

class CffSwscale
{
public:
	CffSwscale();
	~CffSwscale();

private:

	int m_src_w;
	int m_src_h;
	int m_src_bpp;
	uint8_t *m_src_data[4];
	int m_src_linesize[4];
	enum AVPixelFormat m_src_pix_fmt;

	int m_dst_w;
	int m_dst_h;
	int m_dst_bpp;
	uint8_t *m_dst_data[4];
	int m_dst_linesize[4];
	enum AVPixelFormat m_dst_pix_fmt;

	struct SwsContext *m_sws_ctx;

public:
	void SetBase(int src_w, int src_h, enum AVPixelFormat src_pix_fmt,
				 int dst_w, int dst_h, enum AVPixelFormat dst_pix_fmt);

	int GetSrcSizeBpp() { return m_src_w * m_src_h * m_src_bpp / 8; }
	int GetDstSizeBpp() { return m_dst_w * m_dst_h * m_dst_bpp / 8; }
	int Create();
	void Destory();
	int Scale(uint8_t *src_buf, const int src_size, uint8_t *dst_buf, int& dst_size);
	int ScaleX(uint8_t *src_buf, const int src_size, uint8_t *dst_buf, int& dst_size);
};

#endif  // _FF_SWSCALE_H_