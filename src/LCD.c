#include "LCD.h"
#include "main.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

extern spi_device_handle_t lcd_spi;

// 初始化LCD屏幕的GPIO
static void LCD_gpio_init(void)
{
    gpio_config_t gpio_configStruct = {
		/**
		 * PIN32: LED, PIN33: CS, PIN34: DC, PIN35: RESET
		*/
		.pin_bit_mask = GPIO_SEL_32 | GPIO_SEL_33 | GPIO_SEL_25 | GPIO_SEL_26,
		.mode = GPIO_MODE_OUTPUT,  // 输出
		.pull_up_en = 1   // 上拉
	};

    
    gpio_config(&gpio_configStruct);
}

// 初始化LCD屏幕的SPI接口
static void LCD_spi_init(void)
{
    spi_bus_config_t spi_busConfig = {
		.miso_io_num = LCD_MISO_PIN,
		.mosi_io_num = LCD_MOSI_PIN,
		.sclk_io_num = LCD_SCLK_PIN,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};
    spi_device_interface_config_t spi_deviceConfig = {
		.clock_speed_hz = 32000000, // 屏幕的时钟频率是32 MHz
    	.mode = 0,
    	.queue_size = 7    // 队列中最多有七个传输
	};                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    spi_bus_initialize(HSPI_HOST, &spi_busConfig, 0);   // 不使能DMA

    spi_bus_add_device(HSPI_HOST, &spi_deviceConfig, &lcd_spi);
}

// 写LCD寄存器地址
static void LCD_WR_REG(uint8_t data)
{
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data,
        .user = (void*)0
    };
    
    gpio_set_level(LCD_CS_PIN, 0);
    gpio_set_level(LCD_DC_PIN, 0);      // DC为低时写LCD寄存器地址
    spi_device_polling_transmit(lcd_spi, &t);   // SPI传输
    gpio_set_level(LCD_CS_PIN, 1);
}

// 写LCD寄存器数据
static void LCD_WR_DATA(uint8_t data)
{
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data,
        .user = (void*)0
    };
    
    gpio_set_level(LCD_CS_PIN, 0);
    gpio_set_level(LCD_DC_PIN, 1);      // DC为高时写LCD寄存器数据
    spi_device_polling_transmit(lcd_spi, &t);   // SPI传输
    gpio_set_level(LCD_CS_PIN, 1);
}

// 写16位数据
void Lcd_WriteData_16Bit(u16 data)
{
	u8 byte = data >> 8;
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &byte,
        .user = (void*)0
    };

	gpio_set_level(LCD_CS_PIN, 0);
    gpio_set_level(LCD_DC_PIN, 1);      // DC为高时写LCDd寄存器数据
    spi_device_polling_transmit(lcd_spi, &t);   // SPI传输
	byte = data;
	spi_device_polling_transmit(lcd_spi, &t);
	gpio_set_level(LCD_CS_PIN, 1);
}

// 向LCD寄存器内写入数据
static void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

// 硬复位
static void LCD_RESET(void)
{
	gpio_set_level(LCD_RESET_PIN, 0);
	vTaskDelay(100/portTICK_PERIOD_MS);
	gpio_set_level(LCD_RESET_PIN, 1);
	vTaskDelay(50/portTICK_PERIOD_MS);
}

// 准备写入GRAM
static void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(WRAM_CMD);
}	 

// 清屏
void LCD_Clear(u16 Color)
{
    unsigned int i;  
	LCD_SetWindows(0,0,LCD_X-1,LCD_Y-1);   
	gpio_set_level(LCD_CS_PIN, 0);
    gpio_set_level(LCD_DC_PIN, 1); 
	for(i=0;i<LCD_Y*LCD_X;i++)
	{
		Lcd_WriteData_16Bit(Color);
	}
	gpio_set_level(LCD_CS_PIN, 1);
	LCD_SetWindows(0, 0, LCD_X-1, LCD_Y-1);
} 

// 硬件初始化
static void LCD_hw_init(void)
{
    LCD_gpio_init();
    LCD_spi_init();
}

// 设置窗口位置
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(SET_X_CMD);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);

	LCD_WR_REG(SET_Y_CMD);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
}   

// 设置光标位置
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

void LCD_Init(void)
{  
	LCD_hw_init();										 
 	LCD_RESET(); //LCD 复位
//*************2.8inch ILI9341初始化**********//	
	LCD_WR_REG(0xCF);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0xC9); //C1 
	LCD_WR_DATA(0X30); 
	LCD_WR_REG(0xED);  
	LCD_WR_DATA(0x64); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0X12); 
	LCD_WR_DATA(0X81); 
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA(0x85); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x7A); 
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x34); 
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA(0x20); 
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA(0x1B);   //VRH[5:0] 
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA(0x00);   //SAP[2:0];BT[3:0] 01 
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA(0x30); 	 //3F
	LCD_WR_DATA(0x30); 	 //3C
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA(0XB7); 
	LCD_WR_REG(0x36);    // Memory Access Control 
	LCD_WR_DATA(0x08); 
	LCD_WR_REG(0x3A);   
	LCD_WR_DATA(0x55); 
	LCD_WR_REG(0xB1);   
	LCD_WR_DATA(0x00);   
	LCD_WR_DATA(0x1A); 
	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x0E); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x54); 
	LCD_WR_DATA(0XA9); 
	LCD_WR_DATA(0x43); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 		 
	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x15); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x11); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x2B); 
	LCD_WR_DATA(0x56); 
	LCD_WR_DATA(0x3C); 
	LCD_WR_DATA(0x05); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);	 
	LCD_WR_REG(0x11); //Exit Sleep
	vTaskDelay(120/portTICK_PERIOD_MS);
	LCD_WR_REG(0x29); //display on		

    LCD_WriteReg(0x36,(1<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
	gpio_set_level(LCD_LED_PIN, 1);	 
	// LCD_Clear(BLACK);//清全屏
}