/*
 * date: 2017年3月21日13:52:08
 * author: Ifan Tsai
 * describe: x210触摸屏翻页操作
 *                
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <linux/input.h>
#include "ts.h"
#include "picture_manage.h"

void ts_turn_page(unsigned int *pfb, pic_info_list_t *pic_list)
{
	pic_list = pic_list->pnext;

	int fd = open(TOUSH_SCREEN_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror("open failed");
		exit(1);
	}

	struct input_event ts_buffer;
	int len;
	int flag = 1;             // falg置1表示切图了
	while (1)
	{
		if (flag)
		{
			pic_list->show(pfb, &pic_list->pic_info);
			flag = 0;
		}	
		
		bzero(&ts_buffer, sizeof(ts_buffer));
		len = read(fd, &ts_buffer, sizeof(ts_buffer));
		if (len != sizeof(ts_buffer))
		{
			printf("read failed\n");
			exit(1);
		}
		if (ts_buffer.type == EV_ABS && ts_buffer.code == ABS_X)
		{
			// 上翻
			if (0 <= ts_buffer.value && ts_buffer.value <= TOUSH_WIDTH)
			{
				pic_list = pic_list->prev;
				flag = 1;
			}
			// 下翻
			else if (ts_buffer.value >= (WIDTH - TOUSH_WIDTH) &&    \
				      ts_buffer.value <= WIDTH) 
			{
				pic_list = pic_list->pnext;
				flag = 1;
			}
		}
	}


	close(fd);
}


