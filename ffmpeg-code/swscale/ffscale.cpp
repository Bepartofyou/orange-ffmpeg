#include "ffscale.h"
#include "libyuv/convert.h"
#include "libyuv/rotate.h"

CffSwscale::CffSwscale() {}

CffSwscale::~CffSwscale() {}

void CffSwscale::SetBase(int src_w, int src_h, enum AVPixelFormat src_pix_fmt,
						 int dst_w, int dst_h, enum AVPixelFormat dst_pix_fmt) {
	m_src_w = src_w;
	m_src_h = src_h;
	m_src_pix_fmt = src_pix_fmt;
	m_src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(m_src_pix_fmt));

	m_dst_w = dst_w;
	m_dst_h = dst_h;
	m_dst_pix_fmt = dst_pix_fmt;
	m_dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(m_dst_pix_fmt));
}

int CffSwscale::Create() {

	int ret = 0;
	/* create scaling context */
	m_sws_ctx = sws_getContext(m_src_w, m_src_h, m_src_pix_fmt,
		m_dst_w, m_dst_h, m_dst_pix_fmt,
		SWS_BICUBIC /*SWS_BILINEAR*/, NULL, NULL, NULL);
	if (!m_sws_ctx) {
		fprintf(stderr,
			"Impossible to create scale context for the conversion "
			"fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
			av_get_pix_fmt_name(m_src_pix_fmt), m_src_w, m_src_h,
			av_get_pix_fmt_name(m_dst_pix_fmt), m_dst_w, m_dst_h);

		Destory();
		return  AVERROR(EINVAL);
	}

	/* allocate source and destination image buffers */
	if ((ret = av_image_alloc(m_src_data, m_src_linesize,
		m_src_w, m_src_h, m_src_pix_fmt, 16)) < 0) {
		fprintf(stderr, "Could not allocate source image\n");
		
		Destory();
		return  ret;
	}

	/* buffer is going to be written to rawvideo file, no alignment */
	if ((ret = av_image_alloc(m_dst_data, m_dst_linesize,
		m_dst_w, m_dst_h, m_dst_pix_fmt, 1)) < 0) {
		fprintf(stderr, "Could not allocate destination image\n");
		
		Destory();
		return  ret;
	}
}

void CffSwscale::Destory() {

	av_freep(&m_src_data[0]);
	av_freep(&m_dst_data[0]);
	sws_freeContext(m_sws_ctx);
}

int CffSwscale::Scale(uint8_t *src_buf, const int src_size, uint8_t *dst_buf, int& dst_size) {

	if (src_size != GetSrcSizeBpp())
		return -1;
	if (dst_size < GetDstSizeBpp())
		return -1;

	switch (m_src_pix_fmt) {
		case AV_PIX_FMT_GRAY8: {
			memcpy(m_src_data[0], src_buf, m_src_w*m_src_h);
			break;
		}
		case AV_PIX_FMT_YUV420P: {
			memcpy(m_src_data[0], src_buf,							 m_src_w*m_src_h);      //Y  
			memcpy(m_src_data[1], src_buf + m_src_w*m_src_h,		 m_src_w*m_src_h / 4);  //U  
			memcpy(m_src_data[2], src_buf + m_src_w*m_src_h * 5 / 4, m_src_w*m_src_h / 4);  //V  
			break;
		}
		case AV_PIX_FMT_YUV422P: {
			memcpy(m_src_data[0], src_buf,							 m_src_w*m_src_h);      //Y  
			memcpy(m_src_data[1], src_buf + m_src_w*m_src_h,		 m_src_w*m_src_h / 2);  //U  
			memcpy(m_src_data[2], src_buf + m_src_w*m_src_h * 3 / 2, m_src_w*m_src_h / 2);  //V  
			break;
		}
		case AV_PIX_FMT_YUV444P: {
			memcpy(m_src_data[0], src_buf,						 m_src_w*m_src_h);      //Y  
			memcpy(m_src_data[1], src_buf + m_src_w*m_src_h,	 m_src_w*m_src_h);      //U  
			memcpy(m_src_data[2], src_buf + m_src_w*m_src_h * 2, m_src_w*m_src_h);      //V  
			break;
		}
		case AV_PIX_FMT_YUYV422: {
			memcpy(m_src_data[0], src_buf, m_src_w*m_src_h * 2);                  //Packed  
			break;
		}
		case AV_PIX_FMT_RGB24: {
			memcpy(m_src_data[0], src_buf, m_src_w*m_src_h * 3);                  //Packed  
			break;
		}
		case AV_PIX_FMT_RGBA: {
			memcpy(m_src_data[0], src_buf, m_src_w*m_src_h * 4);                  //Packed  
			break;
		}
		default: {
			fprintf(stderr, "Not Support Input Pixel Format.\n");
			return -1;
		}
	}

	/* convert to destination format */
	sws_scale(m_sws_ctx, (const uint8_t * const*)m_src_data,
		m_src_linesize, 0, m_src_h, m_dst_data, m_dst_linesize);

	switch (m_dst_pix_fmt) {
		case AV_PIX_FMT_GRAY8: {
			memcpy(dst_buf, m_dst_data[0], m_dst_w*m_dst_h);
			break;
		}
		case AV_PIX_FMT_YUV420P: {
			memcpy(dst_buf,							  m_dst_data[0], m_dst_w*m_dst_h);
			memcpy(dst_buf + m_dst_w*m_dst_h,		  m_dst_data[1], m_dst_w*m_dst_h / 4);
			memcpy(dst_buf + m_dst_w*m_dst_h * 5 / 4, m_dst_data[2], m_dst_w*m_dst_h / 4);
			break;
		}
		case AV_PIX_FMT_YUV422P: {
			memcpy(dst_buf,											 m_dst_data[0], m_dst_w*m_dst_h);
			memcpy(dst_buf + m_dst_w*m_dst_h,						 m_dst_data[1], m_dst_w*m_dst_h / 2);
			memcpy(dst_buf + m_dst_w*m_dst_h * 3 / 2, m_dst_data[2], m_dst_w*m_dst_h / 2);
			break;
		}
		case AV_PIX_FMT_YUV444P: {
			memcpy(dst_buf,										 m_dst_data[0], m_dst_w*m_dst_h);
			memcpy(dst_buf + m_dst_w*m_dst_h,					 m_dst_data[1], m_dst_w*m_dst_h);
			memcpy(dst_buf + m_dst_w*m_dst_h * 2, m_dst_data[2], m_dst_w*m_dst_h);
			break;
		}
		case AV_PIX_FMT_YUYV422: {
			memcpy(dst_buf, m_dst_data[0], m_dst_w*m_dst_h * 2); //Packed  
			break;
		}
		case AV_PIX_FMT_RGB24: {
			memcpy(dst_buf, m_dst_data[0], m_dst_w*m_dst_h * 3); //Packed
			break;
		}
		default: {
			fprintf(stderr, "Not Support Output Pixel Format.\n");
			return -1;
		}
	}

	return 0;
}

int CffSwscale::ScaleX(uint8_t *src_buf, const int src_size, uint8_t *dst_buf, int& dst_size) {

	if (src_size != GetSrcSizeBpp())
		return -1;
	if (dst_size < GetDstSizeBpp())
		return -1;
	if (m_src_w != m_dst_w || m_src_h != m_dst_h) // the same resolution
		return -1;

	if (AV_PIX_FMT_RGBA == m_src_pix_fmt && AV_PIX_FMT_YUV420P == m_dst_pix_fmt){

		int half_width = (m_src_w + 1) / 2;
		int half_height = (m_src_h + 1) / 2;

		libyuv::ABGRToI420(src_buf, m_src_w * 4,
			dst_buf, m_src_w,
			dst_buf + m_src_w * m_src_h, half_width,
			dst_buf + m_src_w * m_src_h + half_width * half_height, half_width,
			m_src_w, m_src_h);
	}
	else
		return -1;
	
	return 0;
}

