#ifndef __PRIVATE_IMG_INCLUDE_H__
#define __PRIVATE_IMG_INCLUDE_H__


/*********** 图片颜色格式定义 ***********/
#define COLOR_FORMAT_RGB565			1
#define COLOR_FORMAT_RGB888			2
#define COLOR_FORMAT_ARGB8565		3
#define COLOR_FORMAT_ARGB8888		4
#define COLOR_FORMAT_ALPHA			5


/*********** 播放模式定义 ***********/
#define PLAY_NONE					0
#define PLAY_ONCE					1
#define PLAY_LOOP					2


/*********** 控件类型定义 ***********/
#define CTRL_TYPE_WINDOW			2
#define CTRL_TYPE_LAYOUT			3
#define CTRL_TYPE_LAYER				4
#define CTRL_TYPE_GRID				5
#define CTRL_TYPE_LIST				6
#define CTRL_TYPE_BUTTON			7
#define CTRL_TYPE_PIC				8
#define CTRL_TYPE_BATTERY			9
#define CTRL_TYPE_TIME				10
#define CTRL_TYPE_CAMERA			11
#define CTRL_TYPE_TEXT				12
#define CTRL_TYPE_ANIMATION			13
#define CTRL_TYPE_PLAYER			14
#define CTRL_TYPE_NUMBER			15
#define CTRL_TYPE_PROGRESS			20
#define CTRL_TYPE_MULTIPROGRESS		22
#define CTRL_TYPE_WATCH				24
#define CTRL_TYPE_SLIDER			28
#define CTRL_TYPE_VSLIDER			33
#define CTRL_TYPE_COMPASS			38


/*********** 数据源定义 ***********/
#define DATA_SOURCE_NONE			52400


/*********** 包含头文件 ***********/

#include "img_typedef.h"
#include "all_image_id.h"
#include "all_image_info.h"


#endif
