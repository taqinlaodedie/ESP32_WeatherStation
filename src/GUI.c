#include "GUI.h"
#include "LCD.h"
#include "GUI_ressources.h"
#include "string.h"

extern xQueueHandle xQueue;

/*****************************************************************************
 * @name       :void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size)
 * @date       :2018-08-09 
 * @function   :Display a single English character
 * @parameters :x:the bebinning x coordinate of the Character display position
                y:the bebinning y coordinate of the Character display position
								fc:the color value of display character
								bc:the background color of display character
								num:the ascii code of display character(0~94)
								size:the size of display character
 * @retvalue   :None
******************************************************************************/ 
static void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size)
{  
    u8 temp;
    u8 pos,t;   
		   
	num=num-' ';//得到偏移后的值
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//设置单个文字显示窗口
	//非叠加方式
	for(pos=0;pos<size;pos++) {
		if(size==12)temp=asc2_1206[num][pos];//调用1206字体
		else temp=asc2_1608[num][pos];		 //调用1608字体
		for(t=0;t<size/2;t++) {                 
		    if(temp&0x01)Lcd_WriteData_16Bit(fc); 
			else Lcd_WriteData_16Bit(bc); 
			temp>>=1; 	
		}	
	}	
	LCD_SetWindows(0,0,LCD_X-1,LCD_Y-1);//恢复窗口为全屏    	   	 	  
}

/*****************************************************************************
 * @name       :void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p, u16 fc)
 * @date       :2018-08-09 
 * @function   :Display English string
 * @parameters :x:the bebinning x coordinate of the English string
                y:the bebinning y coordinate of the English string
								p:the start address of the English string
								size:the size of display character
                                fc: font color
 * @retvalue   :None
******************************************************************************/   	  
static void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p, u16 fc)
{         
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {   
		if(x>(LCD_X-1)||y>(LCD_Y-1)) 
		return;     
        LCD_ShowChar(x,y,fc,BLUE,*p,size);
        x+=size/2;
        p++;
    }  
} 

 
/*****************************************************************************
 * @name       :void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p)
 * @date       :2018-08-09 
 * @function   :Display a 16-bit BMP image
 * @parameters :x:the bebinning x coordinate of the BMP image
                y:the bebinning y coordinate of the BMP image
								p:the start address of image array
 * @retvalue   :None
******************************************************************************/ 
static void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) //显示38*38图片
{
  	int i; 
	unsigned char picH,picL; 
	LCD_SetWindows(x,y,x+38-1,y+38-1);//窗口设置
    for(i=0;i<38*38;i++)
	{	
	 	picL=*(p+i*2);	//数据低位在前
		picH=*(p+i*2+1);				
		Lcd_WriteData_16Bit(picL|picH<<8);  						
	}	
	LCD_SetWindows(0,0,LCD_X-1,LCD_Y-1);//恢复显示窗口为全屏	
}

static void Gui_background(void) //显示背景图片
{
  	int i; 
	unsigned char picH,picL; 
	LCD_SetWindows(0,0,LCD_X-1,LCD_Y-1);//窗口设置
    for(i=0;i<LCD_X*LCD_Y;i++)
	{	
	 	picL=*(gImage_background+i*2);	//数据低位在前
		picH=*(gImage_background+i*2+1);				
		Lcd_WriteData_16Bit(picL|picH<<8);  						
	}	
	LCD_SetWindows(0,0,LCD_X-1,LCD_Y-1);//恢复显示窗口为全屏	
}

static void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size)
{					
	u16 x0=x;							  	  
  	u8 bHz=0;     //字符或者中文 
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
			if(x>(LCD_X-size/2)||y>(LCD_Y-size)) 
			return; 
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {          
		        if(*str==0x0D)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else
				{
					if(size>16)//字库中没有集成12X24 16X32的英文字体,用8X16代替
					{  
					LCD_ShowChar(x,y,fc,bc,*str,16);
					x+=8; //字符,为全字的一半 
					}
					else
					{
					LCD_ShowChar(x,y,fc,bc,*str,size);
					x+=size/2; //字符,为全字的一半 
					}
				} 
				str++; 
		        
	        }
        }		 
    }   
}

// 绘制第一幅图像
void GUI_init(void)
{
    LCD_Init();
	Gui_background();
    vTaskDelay(50/portTICK_PERIOD_MS);
	Show_Str(CITY_POS_X, CITY_POS_Y, YELLOW, DARKBLUE, (u8 *)"Paris", 16);
    Show_Str(ITEM_POS_X, WEATEHR_POS_Y, GREEN, DARKBLUE, (u8 *)"Weather: ", 16);
    Show_Str(ITEM_POS_X, TEMP_POS_Y, GREEN, DARKBLUE, (u8 *)"Temperature: ", 16);
    Show_Str(ITEM_POS_X, PRESS_POS_Y, GREEN, DARKBLUE, (u8 *)"Pressure: ", 16);
    Show_Str(ITEM_POS_X, HUM_POS_Y, GREEN, DARKBLUE, (u8 *)"Humidity: ", 16);
    Show_Str(ITEM_POS_X, WIND_POS_Y, GREEN, DARKBLUE, (u8 *)"Wind speed: ", 16);
}

// 更新图像
void GUI_update(void)
{
    message_t msg;
    weatherContent_t wMsg;
    u8 buf[20];

    do {
        xQueueReceive(xQueue, (message_t *)&msg, 10);   // 接收消息
        if (msg.ID == WEATHER_MSG) { // 天气信息
			// 显示天气信息
            memcpy(&wMsg, &msg.data, sizeof(weatherContent_t));
            Show_Str(INFO_POS_X, WEATEHR_POS_Y, WHITE, DARKBLUE, wMsg.type, 16);
            sprintf((char *)buf, "%.1f C", (float)((float)wMsg.temp / 10.0));
            Show_Str(INFO_POS_X, TEMP_POS_Y, WHITE, DARKBLUE, buf, 16);
            sprintf((char *)buf, "%d mBar", wMsg.pressure);
            Show_Str(INFO_POS_X, PRESS_POS_Y, WHITE, DARKBLUE, buf, 16);
            sprintf((char *)buf, "%d %%", wMsg.humidity);
            Show_Str(INFO_POS_X, HUM_POS_Y, WHITE, DARKBLUE, buf, 16);
            sprintf((char *)buf, "%.1f m/s", (float)((float)wMsg.wind_speed / 10.0));
            Show_Str(INFO_POS_X, WIND_POS_Y, WHITE, DARKBLUE, buf, 16);

			// 显示天气图标
			if (strstr((char *)wMsg.type, "Clear")) {
				Gui_Drawbmp16(ICON_POS_X, ICON_POS_Y, gImage_downpour);
			}
			else if (strstr((char *)wMsg.type, "Rain")) {
				Gui_Drawbmp16(ICON_POS_X, ICON_POS_Y, gImage_rain);
			}
			else if (strstr((char *)wMsg.type, "Snow")) {
				Gui_Drawbmp16(ICON_POS_X, ICON_POS_Y, gImage_snow);
			}
			else if (strstr((char *)wMsg.type, "Clouds")) {
				Gui_Drawbmp16(ICON_POS_X, ICON_POS_Y, gImage_cloud);
			}
			else if (strstr((char *)wMsg.type, "Thunderstorm")) {
				Gui_Drawbmp16(ICON_POS_X, ICON_POS_Y, gImage_lightning);
			}
			else if (strstr((char *)wMsg.type, "Mist")) {
				Gui_Drawbmp16(ICON_POS_X, ICON_POS_Y, gImage_mist);
			}
        }
    }
    while (1);
}