#include "osapi.h"
#include "driver/uart.h"
#include "user_interface.h"
#include "driver/gpio_key.h"

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
void ICACHE_FLASH_ATTR long_press(void){
    INFO("long_press\n");
}
void ICACHE_FLASH_ATTR short_pres(void){
    INFO("short_pres\n");
}
void ICACHE_FLASH_ATTR key_down(void){
    INFO("key_down\n");
    gpio_write(D4,0);
}
void ICACHE_FLASH_ATTR key_up(void){
    INFO("key_up\n");
    gpio_write(D4,1);
}
void ICACHE_FLASH_ATTR key_init(void){
    //设置按键数量
    set_key_num(2);
    //设置按键长按、短按回调
    key_add(D5,long_press,short_pres);
    //设置按键按下、松开回调
    status_key_add(D6,key_down,key_up);
}

void ICACHE_FLASH_ATTR
user_init(void)
{
	uart_init(115200, 115200);
	os_delay_us(60000);
    
    //GPIO输出初始化
    gpio_out_init(D4,1);
    //按键初始化
    key_init();
}
