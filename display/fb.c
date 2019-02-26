/*
 * date: 2017年3月16日22:30:52
 * author: Ifan Tsai
 * describe: x210开发板lcd相关操作
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include "fb.h"
#include "debug.h"

static struct fb_fix_screeninfo fb_finfo;

int fb_open(const char *fb_path, unsigned int **pfb)
{
	int fd = open(fb_path, O_RDWR);
	if (fd < 0)
	{
		return -1;
	}

	int ret = ioctl(fd, FBIOGET_FSCREENINFO, &fb_finfo);
	if (ret < 0)
	{
		goto err;
	}

	*pfb = mmap(NULL, fb_finfo.smem_len,
						PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (!pfb)
	{
		goto err;
	}

	return fd;
	
err:
	close(fd);
	return -1;
}

inline void fb_close(unsigned int fd, unsigned int *pfb)
{
	munmap(pfb, fb_finfo.smem_len);
	close(fd);
}

void fb_set_bgc(unsigned int *pfb, unsigned int pixel)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			*(pfb + i * WIDTH + j) = pixel;
		}
	}
}

void fb_show_picture_bmp(unsigned int *pfb, const pic_info_t *pic_info)
{
	unsigned char *pdata = pic_info->buf;
	// 传递过来的图像有效数据是从左下方开始扫描的
	int num = pic_info->width * pic_info->height * pic_info->bpp / 8 - 1 - 2;
	for (int i = 0; i < pic_info->height; i++)
	{
		for (int j = pic_info->width - 1; j >= 0; j--)
		{
			*(pfb + i * WIDTH + j) =           \
				(pdata[num] << 0) |            \
				(pdata[num + 1] << 8) |        \
				(pdata[num + 2] << 16);
			num -= 3;
		}
	}
	//while (1);
}

void fb_show_picture_jpg(unsigned int *pfb, const pic_info_t *pic_info)
{
	unsigned char *pdata = pic_info->buf;
	int num = 0;
	for (int i = 0; i < pic_info->height; i++)
	{
		for (int j = 0; j < pic_info->width; j++)
		{
			*(pfb + i * WIDTH + j) =           \
				(pdata[num] << 16) |           \
				(pdata[num + 1] << 8) |        \
				(pdata[num + 2] << 0);
			num += 3;
		}
	}
}
