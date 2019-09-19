/*
 * webconfig
 * Author: HoGC 
 */
#include "osapi.h"
#include "driver/uart.h"
#include "user_interface.h"
#include "driver/webserver.h"

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

void ICACHE_FLASH_ATTR webserver_cb(char *pusrdata, unsigned short length) {

}

void wifi_handle_event_cb(System_Event_t *evt)
{
    switch (evt->event){
    case EVENT_STAMODE_CONNECTED:
        INFO("start webserver\n"); 
        webserver_init(webserver_cb); 
        break;
    default:
        break;
    }
}


void ICACHE_FLASH_ATTR
user_init(void)
{
	uart_init(115200, 115200);
	os_delay_us(60000);

    wifi_set_opmode(0x01); //设置为STATION模式
	struct station_config stationConf;
    os_strcpy(stationConf.ssid, "wifi_ssid");	  //改成你自己的路由器的用户名
	os_strcpy(stationConf.password, "wifi_password"); //改成你自己的路由器的密码
	wifi_station_set_config(&stationConf); //设置WiFi station接口配置，并保存到 flash
	wifi_station_connect();

    wifi_set_event_handler_cb(wifi_handle_event_cb);
}
