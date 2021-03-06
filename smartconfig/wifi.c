/*
 * wifi driver
 * Author: HoGC 
 */
#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "user_config.h"
#include "smartconfig.h"
#include "airkiss.h"
#include "driver/wifi.h"

#define WIFI_DEBUG_ON

#if defined(WIFI_DEBUG_ON)
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

smartconfig_cd_t finish_cd = NULL;
wifconnect_cb_t w_connect = NULL;
wifdisconnect_cb_t w_disconnect = NULL;

LOCAL esp_udp ssdp_udp;
LOCAL struct espconn pssdpudpconn;
LOCAL os_timer_t ssdp_time_serv;
uint8_t lan_buf[200];
uint16_t lan_buf_len;
uint8 udp_sent_cnt = 0;
uint8 smartconfig_flag = 0;
bool connect_flag = 0;
os_timer_t OS_Timer_SM;
os_timer_t OS_Timer_Wifichange;

const airkiss_config_t akconf = { (airkiss_memset_fn) &memset,
		(airkiss_memcpy_fn) &memcpy, (airkiss_memcmp_fn) &memcmp, 0, };
static sm_status sm_comfig_status = SM_STATUS_TIMEOUT;


LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_time_callback(void) {
	uint16 i;
	airkiss_lan_ret_t ret;

	if ((udp_sent_cnt++) > 30) {
		udp_sent_cnt = 0;
		os_timer_disarm(&ssdp_time_serv); //s
		//return;
	}
	ssdp_udp.remote_port = DEFAULT_LAN_PORT;
	ssdp_udp.remote_ip[0] = 255;
	ssdp_udp.remote_ip[1] = 255;
	ssdp_udp.remote_ip[2] = 255;
	ssdp_udp.remote_ip[3] = 255;
	lan_buf_len = sizeof(lan_buf);
	ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
	DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
	if (ret != AIRKISS_LAN_PAKE_READY) {
		INFO("Pack lan packet error!");
		return;
	}

	ret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
	if (ret != 0) {
		INFO("UDP send error!");
	}
	INFO("Finish send notify!\n");
}

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_recv_callbk(void *arg, char *pdata, unsigned short len) {
	uint16 i;
	remot_info* pcon_info = NULL;

	airkiss_lan_ret_t ret = airkiss_lan_recv(pdata, len, &akconf);
	airkiss_lan_ret_t packret;

	switch (ret) {
	case AIRKISS_LAN_SSDP_REQ:
		espconn_get_connection_info(&pssdpudpconn, &pcon_info, 0);
		INFO("remote ip: %d.%d.%d.%d \r\n", pcon_info->remote_ip[0],
				pcon_info->remote_ip[1], pcon_info->remote_ip[2],
				pcon_info->remote_ip[3]);
		INFO("remote port: %d \r\n", pcon_info->remote_port);

		pssdpudpconn.proto.udp->remote_port = pcon_info->remote_port;
		os_memcpy(pssdpudpconn.proto.udp->remote_ip, pcon_info->remote_ip, 4);
		ssdp_udp.remote_port = DEFAULT_LAN_PORT;

		lan_buf_len = sizeof(lan_buf);
		packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
		DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);

		if (packret != AIRKISS_LAN_PAKE_READY) {
			INFO("Pack lan packet error!");
			return;
		}

		INFO("\r\n\r\n");
		for (i = 0; i < lan_buf_len; i++)
			INFO("%c", lan_buf[i]);
		INFO("\r\n\r\n");

		packret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
		if (packret != 0) {
			INFO("LAN UDP Send err!");
		}

		break;
	default:
		INFO("Pack is not ssdq req!%d\r\n", ret);
		break;
	}
}

void ICACHE_FLASH_ATTR
airkiss_start_discover(void) {
	ssdp_udp.local_port = DEFAULT_LAN_PORT;
	pssdpudpconn.type = ESPCONN_UDP;
	pssdpudpconn.proto.udp = &(ssdp_udp);
	espconn_regist_recvcb(&pssdpudpconn, airkiss_wifilan_recv_callbk);
	espconn_create(&pssdpudpconn);

	os_timer_disarm(&ssdp_time_serv);
	os_timer_setfn(&ssdp_time_serv,
			(os_timer_func_t *) airkiss_wifilan_time_callback, NULL);
	os_timer_arm(&ssdp_time_serv, 1000, 1);		//1s
}

/**
 * Smartconfig 状态处理 
 * @param  status: 状态
 * @param  *pdata: AP数据
 * @retval None
 */
void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata) {
	switch (status) {
	case SC_STATUS_WAIT:
		INFO("SC_STATUS_WAIT\n");
		break;
	case SC_STATUS_FIND_CHANNEL:
		INFO("SC_STATUS_FIND_CHANNEL\n");
		break;
	case SC_STATUS_GETTING_SSID_PSWD:
		INFO("SC_STATUS_GETTING_SSID_PSWD\n");
		sc_type *type = pdata;
		if (*type == SC_TYPE_ESPTOUCH) {
			INFO("SC_TYPE:SC_TYPE_ESPTOUCH\n");
		} else {
			INFO("SC_TYPE:SC_TYPE_AIRKISS\n");
		}
		break;
	case SC_STATUS_LINK:
		INFO("SC_STATUS_LINK\n");
		sm_comfig_status = SM_STATUS_GETINFO;
		struct station_config *sta_conf = pdata;
		wifi_station_set_config(sta_conf);
		wifi_station_disconnect();
		wifi_station_connect();
		break;
	case SC_STATUS_LINK_OVER:
		sm_comfig_status = SM_STATUS_FINISH;
		INFO("SC_STATUS_LINK_OVER\n");
		if (pdata != NULL) {
			//SC_TYPE_ESPTOUCH
			uint8 phone_ip[4] = { 0 };
			os_memcpy(phone_ip, (uint8*) pdata, 4);
			INFO("Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1],
					phone_ip[2], phone_ip[3]);
		} else {
			//SC_TYPE_AIRKISS - support airkiss v2.0
			airkiss_start_discover();
		}
		smartconfig_stop();
		smartconfig_flag = 0;
		os_timer_arm(&OS_Timer_Wifichange, 5000, 1);  // 使能定时器
		os_timer_disarm(&OS_Timer_SM);	// 关闭定时器
		finish_cd(sm_comfig_status);
		break;
	}

}

void wifi_handle_event_cb(System_Event_t *evt)
{
    switch	(evt->event)	{
    case EVENT_STAMODE_GOT_IP:
        INFO("connect to ssid %s\n",evt->event_info.connected.ssid);
		connect_flag = 1;
		if(w_connect != NULL){
			w_connect();
		}
		os_timer_disarm(&OS_Timer_Wifichange);	// 关闭定时器
        break;
    case EVENT_STAMODE_DISCONNECTED:
        INFO("disconnect from	ssid %s\n",evt->event_info.disconnected.ssid);
		if(connect_flag == 1){
			connect_flag = 0;
			if(w_disconnect != NULL){
				w_disconnect();
			}
			os_timer_arm(&OS_Timer_Wifichange, 5000, 1);  // 使能定时器
		}
        break;
    default:
        break;
    }
}

/**
 * WIFI连接状态检查   
 * @retval None
 */
void ICACHE_FLASH_ATTR wifi_ap_change(void) {

	if(get_wifi_connect_status() == 0){
		uint8 wifi_mode;
		wifi_mode = wifi_get_opmode();
		if((wifi_mode == STATION_MODE) || (wifi_mode == STATIONAP_MODE)){
			struct station_config config[5];
			int info_count = wifi_station_get_ap_info(config);
			if(info_count > 1 ){
				int ap_id;
				ap_id = wifi_station_get_current_ap_id();
				ap_id = ++ap_id % info_count;
				INFO("AP_ID : %d", ap_id);
				wifi_station_disconnect();
				wifi_station_ap_change(ap_id);
			}
		}
	}else{
		os_timer_disarm(&OS_Timer_Wifichange);	// 关闭定时器
	}
	
	
}

bool ICACHE_FLASH_ATTR get_wifi_connect_status(void){
	return connect_flag;
}


/**
 * 限制SmartConfig配置时间  
 * @retval 
 */
void ICACHE_FLASH_ATTR sm_wait_time() {

	LOCAL wait_wait = 0;
	if (wait_wait == 60) {
		wait_wait = 0;
		smartconfig_stop();		// 停止SmartConfig
		wifi_station_connect();					// ESP8266连接WIFI
		if (sm_comfig_status != SM_STATUS_GETINFO) {
			sm_comfig_status = SM_STATUS_TIMEOUT;
		}
		smartconfig_flag = 0;
		connect_flag = 0;
		os_timer_disarm(&OS_Timer_SM);	// 关闭定时器
		finish_cd(sm_comfig_status);
		os_timer_arm(&OS_Timer_Wifichange, 5000, 1);  // 使能定时器
	}
	wait_wait++;
}

/**
 * 开始Smartconfig配置  
 * @param  cd: Smartconfig状态回调
 * @retval None
 */
void ICACHE_FLASH_ATTR start_smartconfig(smartconfig_cd_t cd) {
	smartconfig_flag = 1;
	os_timer_disarm(&OS_Timer_Wifichange);	// 关闭定时器
	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS); //SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
	wifi_station_disconnect();
	wifi_set_opmode(STATION_MODE);
	finish_cd = cd;
	smartconfig_start(smartconfig_done);

	if(connect_flag == 1){
		w_disconnect();
		connect_flag = 0;
	}

	os_timer_disarm(&OS_Timer_SM);	// 关闭定时器
	os_timer_setfn(&OS_Timer_SM, (os_timer_func_t *) sm_wait_time, NULL);// 设置定时器
	os_timer_arm(&OS_Timer_SM, 1000, 1);  // 使能定时器
}

/**
 * 连接目标AP
 * @param  ssid: 名字
 * @param  pass: 密码
 * @retval None
 */
void ICACHE_FLASH_ATTR wifi_connect(uint8_t* ssid, uint8_t* pass){

	struct station_config stationConf;

	INFO("WIFI_INIT\r\n");
	wifi_set_opmode_current(STATION_MODE);

	os_memset(&stationConf, 0, sizeof(struct station_config));

	os_sprintf(stationConf.ssid, "%s", ssid);
	os_sprintf(stationConf.password, "%s", pass);

	wifi_station_set_config_current(&stationConf);

	wifi_station_connect();
}

/**
 * 设置wifi连接、断开回调，并自动尝试连接以储存的多个WIFI 
 * @param  u_connect_cb: 连接回调函数
 * @param  u_disconnect_cb: 断开连接回调函数
 * @retval None
 */
void ICACHE_FLASH_ATTR set_wifistate_cb(wifconnect_cb_t u_connect_cb, wifdisconnect_cb_t u_disconnect_cb){

	w_connect = u_connect_cb;
	w_disconnect = u_disconnect_cb;

	int i;
	struct station_config config[5];

	//设置wifi信息存储数量
	wifi_station_ap_number_set(AP_INFO_MAX);

	int ap_info_count = wifi_station_get_ap_info(config);
	INFO("ap_info_count = %d\n", ap_info_count);
	for(i = 0; i < ap_info_count; i++)
	{
		INFO("AP%d\n", i);
		INFO("ssid : %s\n", config[i].ssid);
		INFO("password : %s\n", config[i].password);
	}

	wifi_station_connect(); 

	wifi_set_event_handler_cb(wifi_handle_event_cb);

	os_timer_disarm(&OS_Timer_Wifichange);	// 关闭定时器
	os_timer_setfn(&OS_Timer_Wifichange, (os_timer_func_t *) wifi_ap_change,NULL);// 设置定时器
	os_timer_arm(&OS_Timer_Wifichange, 5000, 1);  // 使能定时器
}
