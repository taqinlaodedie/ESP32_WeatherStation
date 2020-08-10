#ifndef __GUI_H_
#define __GUI_H_

#include "main.h"
#include "http.h"

// 屏幕各元素的坐标
#define ICON_POS_X      30
#define CITY_POS_X      30
#define ITEM_POS_X      100
#define INFO_POS_X      220
#define ICON_POS_Y      90
#define CITY_POS_Y      60
#define WEATEHR_POS_Y   60
#define TEMP_POS_Y      90
#define PRESS_POS_Y     120
#define HUM_POS_Y       150
#define WIND_POS_Y      180

void GUI_init(void);
void GUI_update(void);

#endif