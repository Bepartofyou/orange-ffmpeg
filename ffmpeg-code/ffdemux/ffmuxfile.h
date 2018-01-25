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

class CffmuxFile
{
public:
	CffmuxFile();
	~CffmuxFile();

private:
	std::string      m_strInFile;
	std::string      m_strAoutFile;
	std::string      m_strVoutFile;
	FILE* m_pAf;
	FILE* m_pVf;

	int m_iVideoIndex;
	int m_iAudioIndex;
	AVPacket m_sPkt;
	AVFormatContext *m_pInFmtCtx;
	AVBitStreamFilterContext* m_pAudioBsfc;
	AVBitStreamFilterContext* m_pVideoBsfc;

	uint8_t m_uADTS[7];
public:
	void SetBase(const char* infile, const char* audiofile, const char* videofile){
		m_strInFile = infile;
		m_strAoutFile = audiofile;
		m_strVoutFile = videofile;
	}

	int Create();
	int Demux();
	void Destory();

};

#endif // _FF_MUXFILE_H_