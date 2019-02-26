/*
 * date: 2017��3��20��21:57:47
 * author: Ifan Tsai
 * describe: ɨ��x210�������rootfs��ָ��Ŀ¼��ͼƬ���������������
 *           �Լ���ʾ������������ͼƬ
 *          
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "picture_manage.h"
#include "fb_bmp.h"
#include "fb_jpeg.h"
#include "fb_png.h"
#include "debug.h"

// ɨ��ָ��Ŀ¼�µ�bmp��jpg��png��ʽ��ͼƬ��ŵ�������
void scan_picture(pic_info_list_t *pic_list, const char *path)
{
	DIR *pdir = opendir(path);
	if (!pdir)
	{
		perror("opendir");
		return;
	}

	struct dirent *dir_info;
	char next_path[1024]   = { 0 };
	struct stat stat_info   = { 0 };
	while ((dir_info = readdir(pdir)) != NULL)
	{
		// ���� . �� ..
		if(!strcmp(dir_info->d_name, ".") || !strcmp(dir_info->d_name, ".."))
		{
			continue;
		}

		memset(next_path, 0, sizeof(next_path));
		strcpy(next_path, path);
		strcat(next_path, "/");
		strcat(next_path, dir_info->d_name);
		/* ͨ��man�ֲ��֪dir_info.d_type����֧�����и�ʽ���ļ�ϵͳ
		 * (nfs�Ͳ�֧��)�������������stat���ж��Ƿ�����ͨ�ļ��Լ�Ŀ¼
		 */
		stat(next_path, &stat_info);
		// �������ͨ�ļ���������ж��Ƿ���ͼƬ�ļ���������뵽pic_list��
		if (S_ISREG(stat_info.st_mode))          
		{
			int fd = open(next_path, O_RDONLY);
			if (is_bmp(fd))
			{
				pic_info_list_t *p =  \
					(pic_info_list_t *)malloc(sizeof(pic_info_list_t));
				p->pnext = NULL;
				strcpy(p->pic_info.pathname, next_path);
				p->type = PIC_TYPE_BMP;
				p->show = show_bmp;

				pic_info_list_t *tp = pic_list;
				while (tp->pnext)
					tp = tp->pnext;
				tp->pnext = p;
				p->prev   = tp;
			}
			lseek(fd, 0, SEEK_SET);   // �ϴ��жϺ󣬼ǵûع��дָ��λ��
			if (is_jpg(fd))
			{
				pic_info_list_t *p =  \
					(pic_info_list_t *)malloc(sizeof(pic_info_list_t));
				p->pnext = NULL;
				strcpy(p->pic_info.pathname, next_path);
				p->type = PIC_TYPE_JPG;
				p->show = show_jpg;

			    pic_info_list_t *tp = pic_list;	
				while (tp->pnext)
					tp = tp->pnext;
				tp->pnext = p;
				p->prev   = tp;
			}
			lseek(fd, 0, SEEK_SET);
			if (is_png(fd))
			{
				pic_info_list_t *p =  \
					(pic_info_list_t *)malloc(sizeof(pic_info_list_t));
				p->pnext = NULL;
				strcpy(p->pic_info.pathname, next_path);
				p->type = PIC_TYPE_PNG;
				p->show = show_png;

			    pic_info_list_t *tp = pic_list;	
				while (tp->pnext)
					tp = tp->pnext;
				tp->pnext = p;
				p->prev   = tp;
			}
			close(fd);
		}
		// �����Ŀ¼����ݹ�Ŀ¼
		else if (S_ISDIR(stat_info.st_mode))         
		{
			scan_picture(pic_list, next_path);
		}
	}
}

// ��pic_info_list_t����ͷβ�ڵ��������γ�ѭ������
void be_pic_info_loop_list(pic_info_list_t *pictures)
{
	pic_info_list_t *tp = pictures->pnext;
	pic_info_list_t *ttp = tp;
	while (tp->pnext != NULL)
	{
		tp = tp->pnext;
	}
	tp->pnext = ttp;
	ttp->prev = tp;
}

























