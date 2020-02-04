#include "osapi.h"
#include "driver/uart.h"
#include "user_interface.h"

#include "driver/clock.h"

#define MAIN_DEBUG_ON

#if defined(MAIN_DEBUG_ON)
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

#define TASK1_ID        1001
#define TASK2_ID        1002
#define TASK3_ID        1003

os_timer_t test_timer;

uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void){
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


void ICACHE_FLASH_ATTR clock_handler_cd(user_timer_t timer){


	switch (timer.task_id)
	{
		case TASK1_ID:
			INFO("start_task1\n");
			break;
		case TASK2_ID:
			INFO("start_task2\n");
			break;
		case TASK3_ID:
			INFO("start_task3\n");
			break;
	}
}

void ICACHE_FLASH_ATTR clock_test(void){

    int timer_id0;
    int timer_id1;
    int timer_id2;
    clock_time_t time0;
    clock_time_t time1;
    clock_time_t time2;
    /** 解析时间字符串-->time   
     *  可使用格式 
     *   1. "12:1"
     *   2. "12:01"
     *   3. "12:1:0"
     *   4. "12:00:00"
     */
    clock_str_to_time("12:01:00", &time0);
    clock_str_to_time("13:01:00", &time1);
    clock_str_to_time("14:01:00", &time2);
    /**添加定时任务  
     * 时间：time-->"12:01:00"  
     * 重复：星期一（WEEK_MON）、星期五（WEEK_FRI） 
     * 定时时间到了，通过clock_handler_cd回调返回 timer.task_id-->TASK1_ID
     * 返回值定时任务id
     */
    timer_id0 = clock_add_timer(time0, (WEEK_MON | WEEK_FRI), TASK1_ID);
    //添加当天的定时
    timer_id1 = clock_add_today_timer(time1, TASK2_ID);
    //添加每天的定时
    timer_id2 = clock_add_everyday_timer(time2, TASK3_ID);

    //修改定时任务
    clock_str_to_time("12:02:00", &time0);
    clock_set_time_timer(timer_id0, time0);
    //修改定时任务(带修改重复日期)
    clock_str_to_time("12:03:00", &time0);
    clock_set_timer(timer_id0, time0, (WEEK_SUN | WEEK_SAT));
    
    //关闭定时任务
    clock_close_timer(timer_id0);
    //开启定时任务
    clock_open_timer(timer_id0);
    //删除定时任务
    clock_delete_timer(timer_id2);

    //注意: 因有掉电保护功能 每次重启都会添加 实际使用请勿这样使用



    //------------------------------
    //联网校准时间 已自动校准 无需调用 校准周期请修改 TIME_UPDATE_COUNT
    clock_update();
    //获取当前日期和时间
    day_time_t day_time;
    clock_get_day_time(&day_time);



    //-----------------------------
    //获取用于oled显示的字符串  "13.11    Tue    02.04"
    char oled_str[22];
    clock_get_oled_time(oled_str);
    // oled 驱动
    // OLED_ShowStr(0,0,oled_str,1);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200, 115200);
	os_delay_us(60000);

    //联网自动校准时间
    clock_init(clock_handler_cd);

    wifi_set_opmode(0x01); //设置为STATION模式
	struct station_config stationConf;
	os_strcpy(stationConf.ssid, "AP_NAME");	  //改成你自己的路由器的用户名
	os_strcpy(stationConf.password, "AP_PASSWORD"); //改成你自己的路由器的密码
	wifi_station_set_config(&stationConf); //设置WiFi station接口配置，并保存到 flash
	wifi_station_connect();

    clock_test();
}
