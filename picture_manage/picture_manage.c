/*
 * date: 2017年3月20日21:57:47
 * author: Ifan Tsai
 * describe: 扫描x210开发板的rootfs中指定目录的图片并用链表管理起来
 *           以及显示被管理起来的图片
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

// 扫描指定目录下的bmp，jpg，png格式的图片存放到链表中
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
		// 过滤 . 和 ..
		if(!strcmp(dir_info->d_name, ".") || !strcmp(dir_info->d_name, ".."))
		{
			continue;
		}

		memset(next_path, 0, sizeof(next_path));
		strcpy(next_path, path);
		strcat(next_path, "/");
		strcat(next_path, dir_info->d_name);
		/* 通过man手册得知dir_info.d_type并不支持所有格式的文件系统
		 * (nfs就不支持)，所以这里调用stat来判断是否是普通文件以及目录
		 */
		stat(next_path, &stat_info);
		// 如果是普通文件，则进行判断是否是图片文件，是则加入到pic_list中
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
			lseek(fd, 0, SEEK_SET);   // 上次判断后，记得回归读写指针位置
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
		// 如果是目录，则递归目录
		else if (S_ISDIR(stat_info.st_mode))         
		{
			scan_picture(pic_list, next_path);
		}
	}
}

// 将pic_info_list_t链表头尾节点连起来形成循环链表
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

























