/*
 * date: 2017年3月21日17:05:12
 * author: Ifan Tsai
 * describe: 基于x210开发板的图片解码播放器(bmp, jpg, png)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fb.h"
#include "fb_bmp.h"
#include "fb_jpeg.h"
#include "fb_png.h"
#include "picture_manage.h"
#include "ts.h"


int main(int argc, char *argv[])
{
	printf("picture player start...\n");
	unsigned int *pfb;
	int fd = fb_open(FB_PATH, &pfb);
	if (fd < 0)
	{
		perror("fb_open");
		exit(1);
	}

	//fb_set_bgc(pfb, WHITE);
	#if 0
	if (argc == 1)
	{
		printf("arg err: missing picture pathname\n");
		exit(1);
	}
	#endif

	pic_info_list_t *pictures = (pic_info_list_t *)malloc(sizeof(pic_info_list_t));
	pictures->pnext = NULL;
	pictures->prev = NULL;
	scan_picture(pictures, "./picture");
	be_pic_info_loop_list(pictures);

	ts_turn_page(pfb, pictures);

	fb_close(fd, pfb);
	return 0;
}
