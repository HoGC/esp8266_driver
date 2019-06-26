#include "osapi.h"
#include "mem.h"
#include "driver/uart.h"
#include "user_interface.h"
#include "driver/data.h"
#include "driver/ota.h"
#include "mqtt.h"

#define MAIN_DEBUG_ON

#if defined(MAIN_DEBUG_ON)
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

u8 mac_char[13];					//mac地址

//MQTT参数 请在include/mqtt_config.h修改
u8 ota_topic[50]={""};				//ota升级话题
u8 lwt_topic[50]={""};				//遗嘱话题

os_timer_t test_timer;
MQTT_Client mqttClient;


uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
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

/**
 * 获取MAC
 */
void ICACHE_FLASH_ATTR get_mac(void) {

	u8 mac[6];
	wifi_get_macaddr(STATION_IF, mac);
	HexToStr(mac_char, mac, 6, 1);
	INFO("mac:%s\n", mac_char);
}

/**
 * 	MQTT连接回调
 */
void mqttConnectedCb(uint32_t *args) {
	MQTT_Client* client = (MQTT_Client*) args;
	INFO("MQTT: Connected\r\n");
	MQTT_Subscribe(client,ota_topic, 0);
	if(updata_status_check()){
		MQTT_Publish(client, ota_topic, "updata_finish", os_strlen("updata_finish"), 0,0);
	}
}

/**
 * 	MQTT断开连接回调
 */
void mqttDisconnectedCb(uint32_t *args) {
	MQTT_Client* client = (MQTT_Client*) args;
	INFO("MQTT: Disconnected\r\n");
}

/**
 * 	MQTT发布消息回调
 */
void mqttPublishedCb(uint32_t *args) {
	MQTT_Client* client = (MQTT_Client*) args;
	INFO("MQTT: Published\r\n");
}

/**
 * 	MQTT接收数据回调
 */
void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len,
		const char *data, uint32_t data_len) {
	char *topicBuf = (char*) os_zalloc(topic_len + 1), *dataBuf =
			(char*) os_zalloc(data_len + 1);

	MQTT_Client* client = (MQTT_Client*) args;

	os_memcpy(topicBuf, topic, topic_len);
	topicBuf[topic_len] = 0;

	os_memcpy(dataBuf, data, data_len);
	dataBuf[data_len] = 0;

	INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);

	//data = {"url"="http://yourdomain.com:9001/ota/"}
	if (os_strcmp(topicBuf, ota_topic) == 0) {
		char url_data[200];
		if(get_josn_str(dataBuf,"url",url_data)){
            INFO("ota_start\n");
            ota_upgrade(url_data,ota_finished_callback);
		}
	}

	os_free(topicBuf);
	os_free(dataBuf);
}

void ICACHE_FLASH_ATTR connect_mqtt(void){
    INFO("connect_mqtt\n");
    MQTT_Connect(&mqttClient);
}

/**
 * 	MQTT初始化
 */
void ICACHE_FLASH_ATTR mqtt_init(void) {
	
	os_sprintf(ota_topic,OTA_TOPIC,mac_char);
	os_sprintf(lwt_topic,LWT_TOPIC,mac_char);

	MQTT_InitConnection(&mqttClient, MQTT_HOST, MQTT_PORT, DEFAULT_SECURITY);
	MQTT_InitClient(&mqttClient, mac_char, MQTT_USER,MQTT_PASS, MQTT_KEEPALIVE, 1);
	MQTT_InitLWT(&mqttClient, lwt_topic, LWT_MESSAGE, 0, 0);
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200, 115200);
	os_delay_us(60000);

    wifi_set_opmode(0x01); //设置为STATION模式
	struct station_config stationConf;
	os_strcpy(stationConf.ssid, "AP_NAME");	  //改成你自己的   路由器的用户名
	os_strcpy(stationConf.password, "AP_PASSWORD"); //改成你自己的   路由器的密码
	wifi_station_set_config(&stationConf); //设置WiFi station接口配置，并保存到 flash
	wifi_station_connect();

    get_mac();
    mqtt_init();
    
    //延时5秒，等待连接wifi后开始连接MQTT升级
    os_timer_disarm(&test_timer);
	os_timer_setfn(&test_timer, (os_timer_func_t *) connect_mqtt, NULL);
	os_timer_arm(&test_timer, 7000, 0);
}
