#include "delay.h"
#include "main.h"
#include "esp_timer.h"

void delay_us(uint32_t us)
{
    uint32_t m = esp_timer_get_time();
    if (us) {
        uint32_t e = (m + us);
        if (m > e) { //overflow
            while (esp_timer_get_time() > e) {
                asm volatile ("nop");
            }
        }
        while (esp_timer_get_time() < e) { 
            asm volatile ("nop");
        }
    }
}