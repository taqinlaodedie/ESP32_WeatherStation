#ifndef __LCD_H_
#define __LCD_H_

#include "main.h"

#define LCD_LED_PIN     32
#define LCD_CS_PIN      33
#define LCD_DC_PIN      25
#define LCD_RESET_PIN   26
#define LCD_MISO_PIN    12
#define LCD_MOSI_PIN    13
#define LCD_SCLK_PIN    14

#define SET_X_CMD       0x2A
#define SET_Y_CMD       0x2B
#define WRAM_CMD        0x2C

#define LCD_X           320
#define LCD_Y           240

//画笔颜色
#define WHITE           0xFFFF
#define BLACK      	    0x0000	  
#define BLUE       	    0x001F  
#define BRED            0XF81F
#define GRED 	        0XFFE0
#define GBLUE		    0X07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define GREEN           0x07E0
#define CYAN            0x7FFF
#define YELLOW          0xFFE0
#define BROWN 			0XBC40 //棕色
#define BRRED 			0XFC07 //棕红色
#define GRAY  			0X8430 //灰色
//GUI颜色

#define DARKBLUE      	0X01CF	//深蓝色
#define LIGHTBLUE      	0X7D7C	//浅蓝色  
#define GRAYBLUE       	0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	0X841F //浅绿色
#define LIGHTGRAY       0XEF5B //浅灰色(PANNEL)
#define LGRAY 			0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE      	0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)

void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_Init(void);
void Lcd_WriteData_16Bit(u16 data);
void LCD_Clear(u16 Color);

#endif