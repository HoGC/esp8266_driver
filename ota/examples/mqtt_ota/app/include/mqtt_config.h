#ifndef __MQTT_CONFIG_H__
#define __MQTT_CONFIG_H__

/*DEFAULT CONFIGURATIONS*/

#define MQTT_HOST			"0.0.0.0"       //HOST or "mqtt.yourdomain.com"
#define MQTT_PORT			1883
#define MQTT_BUF_SIZE		1024
#define MQTT_KEEPALIVE		120	            /*second*/

#define MQTT_USER			""
#define MQTT_PASS			""

#define OTA_TOPIC           "/esp8266/ota/%s"
#define LWT_TOPIC           "/lwt/%s"

#define LWT_MESSAGE         "offline"

#define MQTT_RECONNECT_TIMEOUT 	5	        /*second*/

#define DEFAULT_SECURITY	0
#define QUEUE_BUFFER_SIZE		 		2048

//#define PROTOCOL_NAMEv31	                /*MQTT version 3.1 compatible with Mosquitto v0.15*/
#define PROTOCOL_NAMEv311			        /*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/

#endif // __MQTT_CONFIG_H__
