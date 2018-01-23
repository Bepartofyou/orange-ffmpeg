#include <stdio.h>
#include <stdlib.h>

#include "ffscale.h"

#define  USE_FF 1

int main(int argc, char **argv)
{
	FILE *src_file, *dst_file;

	src_file = fopen("../doc/544x816.rgba", "rb");
	if (!src_file) {
		fprintf(stderr, "Could not open source file!\n");
		exit(1);
	}

	dst_file = fopen("../doc/544x816.yuv", "wb");
	if (!dst_file) {
		fprintf(stderr, "Could not open destination file !\n");
		exit(1);
	}

	CffSwscale obj;
	obj.SetBase(544, 816, AV_PIX_FMT_RGBA, 544, 816, AV_PIX_FMT_YUV420P);
#if USE_FF
	obj.Create();
#endif

	int src_size = obj.GetSrcSizeBpp();
	uint8_t *src_buffer = (uint8_t *)malloc(src_size);
	int dst_size = obj.GetDstSizeBpp();
	uint8_t *dst_buffer = (uint8_t *)malloc(dst_size);

	fread(src_buffer, 1, src_size, src_file);
	dst_size = obj.GetDstSizeBpp();
#if USE_FF
	obj.Scale(src_buffer, src_size, dst_buffer, dst_size);  //ffmpeg
#else
	obj.ScaleX(src_buffer, src_size, dst_buffer, dst_size);   //libyuv
#endif
	fwrite(dst_buffer, 1, dst_size, dst_file);

#if USE_FF
	obj.Destory();
#endif
	fclose(src_file);
    fclose(dst_file);

    return 0;
}
