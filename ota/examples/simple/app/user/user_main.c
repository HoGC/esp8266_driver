#include "osapi.h"
#include "driver/uart.h"
#include "user_interface.h"
#include "driver/ota.h"

#define MAIN_DEBUG_ON

#if defined(MAIN_DEBUG_ON)
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

os_timer_t test_timer;


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
 * 	ota升级回调
 */
void ICACHE_FLASH_ATTR ota_finished_callback(void * arg) {
	struct upgrade_server_info *update = arg;
	if (update->upgrade_flag == true) {
		INFO("OTA  Success ! rebooting!\n");
		system_upgrade_reboot();
	} else {
		INFO("OTA Failed!\n");
	}
}

void ICACHE_FLASH_ATTR start_ota(void){
    INFO("start_ota\n");
    //开始OTA升级（IP 或 域名）
    ota_upgrade("http://yourdomain.com:9000/OTA/",ota_finished_callback);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200, 115200);
	os_delay_us(60000);

    wifi_set_opmode(0x01); //设置为STATION模式
	struct station_config stationConf;
	os_strcpy(stationConf.ssid, "AP_NAME");	  //改成你自己的路由器的用户名
	os_strcpy(stationConf.password, "AP_PASSWORD"); //改成你自己的路由器的密码
	wifi_station_set_config(&stationConf); //设置WiFi station接口配置，并保存到 flash
	wifi_station_connect();

    //延时7秒，等待连接wifi后开始OTA升级
    os_timer_disarm(&test_timer);
	os_timer_setfn(&test_timer, (os_timer_func_t *) start_ota, NULL);
	os_timer_arm(&test_timer, 7000, 0);
}
