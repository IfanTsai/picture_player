/*
 * date: 2017年3月18日17:49:59
 * author: Ifan Tsai
 * describe: bmp格式图片的显示相关操作
 *          (bmp是未压缩的图片格式，所以不需要进行解码)
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"
#include "fb_bmp.h"
#include "fb.h"

// 判断是否是bmp格式的图片，bmp格式图片以BM开头
int is_bmp(int fd)
{
	unsigned char buf[2] = { 0 };
	read(fd, buf, sizeof(buf));
	if (buf[0] == 'B' && buf[1] == 'M')
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

// 显示bmp格式的图片
void show_bmp(unsigned int *pfb, pic_info_t *pinfo)
{
	
	int fd = open(pinfo->pathname, O_RDONLY);
	debug("the picture pathname is %s\n", pinfo->pathname);
	if (fd < 0)
	{
		perror("open bmp");
		goto err;
	}
	#if 0
	if (is_bmp(fd))
	{
		debug("the picture is bmp\n");
	}
	else
	{
		debug("the picture is not bmp\n");
		goto err;
	}
	#else
		lseek(fd, 2, SEEK_SET);      // bmp头两个字节为格式标识:'B' 'M'
	#endif
	
	// 读取bmp文件头 (文件大小和有效数据的偏移地址)
	ClBitMapFileHeader bmp_head;
	if (read(fd, &bmp_head, sizeof(bmp_head)) < 0)
	{
		perror("read bmp_header");
		goto err;
	}
	debug("the picture size is %ld\n", bmp_head.bfSize);
	debug("the picture bfOffBits is %ld\n", bmp_head.bfOffBits);

    // 读取bmp信息头
	ClBitMapInfoHeader bmp_data;
	if (read(fd, &bmp_data, sizeof(bmp_data)) < 0)
	{
		perror("read bmp_data");
		goto err;
	}
	
	// 将读写指针移动到图片有效数据处
	if (lseek(fd, bmp_head.bfOffBits, SEEK_SET) < 0)
	{
		perror("lseek bfOffBits");
		goto err;
	}
	long height = bmp_data.biHeight;
	long width  = bmp_data.biWidth;
	int  bpp    = bmp_data.biBitCount;
	long len    = height * width * bpp / 8;
	pinfo->buf  = (unsigned char *)malloc(len);
	if (read(fd, pinfo->buf, len) < 0)          // 读取有效数据
	{
		perror("read bmp_data");
		goto err;
	}
	// 填充传递过来的pinfo结构体
	pinfo->bpp    = bpp;
	pinfo->height = height;
	pinfo->width  = width;
	debug("the picture width is %ld\n", width);
	debug("the picture height is %ld\n", height);
	
	fb_show_picture_bmp(pfb, pinfo);               // 将有效数据放到fb中
	
	free(pinfo->buf);
	
err: 
	close(fd);
	
}

















