#include "osapi.h"
#include "driver/uart.h"
#include "user_interface.h"
#include "driver/i2c_oled.h"

#define MAIN_DEBUG_ON

#if defined(MAIN_DEBUG_ON)
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}


void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200, 115200);
	os_delay_us(60000);

	OLED_Init();

    //显示字符串，以左起第0个、上起的第0行为起点、显示字符oled  examples！，使用大字体，分辨率为8*16
	OLED_ShowStr(0,0," oled  examples ",2);
	//显示BMP图片，第一行第40个到第八行第48个，分辨率48*48,显示图片i2c.oled_fonts.h-->BMP[0]
	OLED_DrawBMP(40,2,40+48,8,0);

}
