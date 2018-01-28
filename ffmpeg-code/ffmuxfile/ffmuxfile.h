#ifndef _FF_MUXFILE_H_
#define _FF_MUXFILE_H_

#include <string>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#define XX_MAX_PTS (int64_t)(UINT64_C(0x8000000000000000)-1)
#define FF_MOV_FLAG_FASTSTART   (1 <<  7)

// a wrapper around a single output AVStream
typedef struct OutputStream  {
	int input_index;
	int output_index;

	AVStream *input_st;
	AVFormatContext *input_fmtctx;

	/* pts of the next frame that will be generated */
	int64_t next_pts;
	int64_t offset;

	int64_t last_pts;

	AVPacket pkt;
	int samples_count;
	int frames_count;

	AVBitStreamFilterContext* bsfc;

} OutputStream;


class CffmuxFile
{
public:
	CffmuxFile();
	~CffmuxFile();

private:
	OutputStream m_sVideoOst;
	OutputStream m_sAudioOst;

	AVDictionary *m_pOpt;
	AVOutputFormat *m_pOutFmt;
	AVFormatContext *m_pOutFmtCtx;

	bool m_bHaveVideo;
	bool m_bHaveAudio;
	bool m_bReadVideo;
	bool m_bReadAudio;

	bool m_bH264Video;

	std::string m_strAudioFile;
	std::string m_strVideoFile;
	std::string m_strOutFile;
public:
	void SetBase(const char* audiofile, const char* videofile, const char* outfile, bool bvideoH264);
	int Create();
	int Muxer();
	void Destory();

private:

	void closeStream(OutputStream  *ost);
	void addStream(AVFormatContext *ofmt_ctx, OutputStream  *ost, const char* inputfilename, bool baudio);
	void initVideo();
	void initAudio();
	int writeVideoFrame();
	int writeAudioFrame();
	void logPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt);

};

#endif // _FF_MUXFILE_H_