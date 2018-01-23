#include <stdio.h>
#include <stdlib.h>

#include "ffvencode.h"

int main(int argc, char **argv)
{
	int width = 544, height = 816;

	FILE *f_in = fopen("../doc/544x816.yuv", "rb");
	if (!f_in) {
		fprintf(stderr, "Could not open ../doc/544x816.yuv\n");
		exit(1);
	}

	FILE *f = fopen("../doc/544x816.h264", "wb");
	if (!f) {
		fprintf(stderr, "Could not open ../doc/544x816.h264\n");
		exit(1);
	}

	int tsize = width * height * 3 / 2;

	fseek(f_in, 0, SEEK_END);
	int num = ftell(f_in) / tsize;
	fseek(f_in, 0, SEEK_SET);

	uint8_t* yuvbuff = (uint8_t*)malloc(tsize);
	uint8_t* h264buff = (uint8_t*)malloc(tsize);

	CffVencode obj;
	obj.SetBase(width, height, 10, 1200000, { 1,25 }, { 25,1 }, AV_PIX_FMT_YUV420P);
	obj.Create("libx264");
	
    int i;
    /* encode 1 second of video */
    for (i = 0; i < num; i++) {
        fflush(stdout);

		fread(yuvbuff, 1, tsize, f_in);
        /* encode the image */
		int h264size = tsize;
		std::vector<int> vsize;
		obj.Encode(yuvbuff, tsize, h264buff, h264size, vsize);
		fprintf(stderr, "vector size: %d\n",vsize.size());

		fwrite(h264buff, 1, h264size, f);
    }

    /* flush the encoder */
	int h264size = tsize;
	std::vector<int> vsize;
	obj.Encode(NULL, tsize, h264buff, h264size, vsize);
	fprintf(stderr, "vector size: %zd\n", vsize.size());

	fwrite(h264buff, 1, h264size, f);

	fclose(f_in);
    fclose(f);

    return 0;
}
