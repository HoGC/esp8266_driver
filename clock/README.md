# ESP8266 时钟 闹钟 定时任务 驱动

ESP8266 NONOS SDK 时钟 闹钟 定时任务 驱动    
1. 自动联网校准日期时间
2. 可添加定时任务，用于闹钟、任务等操作
3. 定时任务支持重复，可按照星期几重复
4. 对定时任务进行增删改查 
5. 定时任务支持掉电存储定时任务信息，防止掉电失效

```C
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
    int timer_id3;
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

    /* 解析重复周期字符串
     * "1111111"  7位  周一到周日
     * "today"    今天
     */    
    uint8_t week_bit;
    clock_str_to_week_bit("1111100", &week_bit);
    //添加周一到周五的
    timer_id1 = clock_add_timer(time0, week_bit, TASK2_ID);

    //添加当天的定时
    timer_id2 = clock_add_today_timer(time1, TASK2_ID);
    //添加每天的定时
    timer_id3 = clock_add_everyday_timer(time2, TASK3_ID);

    // //修改定时任务
    // clock_str_to_time("12:02:00", &time0);
    // clock_set_time_timer(timer_id0, time0);
    // //修改定时任务(带修改重复日期)
    // clock_str_to_time("12:03:00", &time0);
    // clock_set_timer(timer_id0, time0, (WEEK_SUN | WEEK_SAT));
    
    //关闭定时任务
    clock_close_timer(timer_id0);
    //开启定时任务
    clock_open_timer(timer_id0);
    //删除定时任务
    clock_delete_timer(timer_id2);

    //注意: 因有掉电保护功能 每次重启都会添加 但无法添加相同时间和相同任务id的定时 

    //------------------------------
    //联网校准时间 已自动校准 无需调用 校准周期请修改 TIME_UPDATE_COUNT
    clock_update();
    //获取当前日期和时间
    day_time_t day_time;
    clock_get_day_time(&day_time);

    //通过id获取定时任务信息
    // {
	//     "id"：1000,
	//     "status": 1,
	//     "time": "17:45:00",
	//     "week": "1111110",
	//     "taskId": 202
    // }
    char timer_itme_info[256];
    clock_get_timer_task_item_info_josn(timer_id1, timer_itme_info);

    // 获取全部定时任务信息
    // {
    //     "timerNum": 2,
    //     "timerInfo": [
    //         {
    //             "id": 1000,
    //             "status": 1,
    //             "time": "17:45:00",
    //             "week": "1111110",
    //             "taskId": 202
    //         },
    //         {
    //             "id": 1001,
    //             "status": 1,
    //             "time": "20:50:00",
    //             "week": "today",
    //             "taskId": 203
    //         }
    //     ]
    // }
    char timer_info[1024];
    clock_get_timer_task_info_json(timer_info);

    //-----------------------------
    //获取用于oled显示的字符串  "13.11    Tue    02.04"
    char oled_str[22];
    clock_get_oled_time(oled_str);
    // oled 驱动
    // OLED_ShowStr(0,0,oled_str,1);
}
````