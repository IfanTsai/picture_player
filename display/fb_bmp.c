/*
 * date: 2017��3��18��17:49:59
 * author: Ifan Tsai
 * describe: bmp��ʽͼƬ����ʾ��ز���
 *          (bmp��δѹ����ͼƬ��ʽ�����Բ���Ҫ���н���)
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

// �ж��Ƿ���bmp��ʽ��ͼƬ��bmp��ʽͼƬ��BM��ͷ
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

// ��ʾbmp��ʽ��ͼƬ
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
		lseek(fd, 2, SEEK_SET);      // bmpͷ�����ֽ�Ϊ��ʽ��ʶ:'B' 'M'
	#endif
	
	// ��ȡbmp�ļ�ͷ (�ļ���С����Ч���ݵ�ƫ�Ƶ�ַ)
	ClBitMapFileHeader bmp_head;
	if (read(fd, &bmp_head, sizeof(bmp_head)) < 0)
	{
		perror("read bmp_header");
		goto err;
	}
	debug("the picture size is %ld\n", bmp_head.bfSize);
	debug("the picture bfOffBits is %ld\n", bmp_head.bfOffBits);

    // ��ȡbmp��Ϣͷ
	ClBitMapInfoHeader bmp_data;
	if (read(fd, &bmp_data, sizeof(bmp_data)) < 0)
	{
		perror("read bmp_data");
		goto err;
	}
	
	// ����дָ���ƶ���ͼƬ��Ч���ݴ�
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
	if (read(fd, pinfo->buf, len) < 0)          // ��ȡ��Ч����
	{
		perror("read bmp_data");
		goto err;
	}
	// ��䴫�ݹ�����pinfo�ṹ��
	pinfo->bpp    = bpp;
	pinfo->height = height;
	pinfo->width  = width;
	debug("the picture width is %ld\n", width);
	debug("the picture height is %ld\n", height);
	
	fb_show_picture_bmp(pfb, pinfo);               // ����Ч���ݷŵ�fb��
	
	free(pinfo->buf);
	
err: 
	close(fd);
	
}

















