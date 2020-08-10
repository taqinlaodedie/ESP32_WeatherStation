#ifndef __MAIN_H_
#define __MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define WEATHER_MSG 0

// 任务发送的消息结构
typedef struct msgStruct {
    u8 ID;
    u8 data[64];
} message_t;

#endif