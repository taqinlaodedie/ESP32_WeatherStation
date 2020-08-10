#ifndef __HTTP_H_
#define __HTTP_H_

#include "main.h"

#define WEB_SERVER "api.openweathermap.org"
#define WEB_PORT 80
#define WEB_URL "http://api.openweathermap.org/data/2.5/weather?q=paris&appid=67c8bd714980aa5a295a559033d822d0"

// 天气消息的数据结构
typedef struct weatherMsgContentStruct {
    u8      type[16];
    u16     temp;
    u16     pressure;
    u8      humidity;
    u8      wind_speed;
} weatherContent_t;

void get_weather_task(void);

#endif