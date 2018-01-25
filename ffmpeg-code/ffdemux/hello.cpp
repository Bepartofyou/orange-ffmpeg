#include <stdlib.h>
#include <stdio.h>
#include "ffmuxfile.h"

/*
*  mpeg4-aac.h/mpeg4-aac.c  from github project
*  url: https://github.com/ireader/media-server
*/

int main(int argc, char **argv)
{
	CffmuxFile obj;
	obj.SetBase("../doc/test.mp4", "../doc/test.aac", "../doc/test.h264");
	obj.Create();
	obj.Demux();
	obj.Destory();

	return 0;
}