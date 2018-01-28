#include <stdlib.h>
#include <stdio.h>
#include "ffmuxfile.h"

int main(int argc, char **argv)
{
	CffmuxFile obj1;
	obj1.SetBase("../doc/1.aac", "../doc/1.mp4", "../doc/2.mp4", false);
	obj1.Create();
	obj1.Muxer();
	obj1.Destory();

	CffmuxFile obj2;
	obj2.SetBase("../doc/1.aac", "../doc/1.h264", "../doc/3.mp4", true); // without B frames
	obj2.Create();
	obj2.Muxer();
	obj2.Destory();
	return 0;
}