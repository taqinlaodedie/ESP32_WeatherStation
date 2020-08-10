#include "main.h"
#include "LCD.h"
#include "driver/spi_master.h"
#include "wifi.h"
#include "http.h"
#include "nvs_flash.h"
#include "GUI.h"

spi_device_handle_t lcd_spi;
QueueHandle_t xQueue;   // 任务的消息队列

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    GUI_init();
    xQueue = xQueueCreate(5, sizeof(message_t)); // 创建任务的消息队列

    xTaskCreate((TaskFunction_t)get_weather_task, "get_weather_task", 4096, NULL, 5, NULL);
    xTaskCreate((TaskFunction_t)GUI_update, "update_GUI", 4096, NULL, 1, NULL);
}