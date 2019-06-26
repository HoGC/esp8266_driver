#include "osapi.h"
#include "driver/uart.h"
#include "user_interface.h"
#include "driver/wifi.h"

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

/**
 * 	smartconfig配置回调
 */
void ICACHE_FLASH_ATTR smartconfig_cd(sm_status status){

	switch (status)
	{
		case SM_STATUS_FINISH:
			INFO("smartconfig_finish\n");
			break;
		case SM_STATUS_GETINFO:
			INFO("wifiinfo_error\n");
			break;
		case SM_STATUS_TIMEOUT:
			INFO("smartconfig_timeout\n");
			break;
	}
}

/**
 * 	WIFI连接回调
 */
void wifi_connect_cb(void){

	INFO("wifi connect!\r\n");
}

/**
 * 	WIFI断开回调
 */
void wifi_disconnect_cb(void){

	INFO("wifi disconnect!\r\n");
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200, 115200);
	os_delay_us(60000);

    wifi_set_opmode(STATION_MODE); 
	//设置wifi信息最大存储数量
	wifi_station_ap_number_set(2);
    //开始smartconfig
    start_smartconfig(smartconfig_cd);
    INFO("start_smartconfig\n");
    //设置wifi连接、断开回调，并自动尝试连接以储存的多个WIFI
    set_wifistate_cb(wifi_connect_cb, wifi_disconnect_cb);
}
