/*
 * webconfig driver
 * Author: HoGC 
 */
#ifndef __WEBCONFIG_H__
#define __WEBCONFIG_H__

typedef void (*config_cd_t)(char *pusrdata, unsigned short length);
extern u8 location[20];

#define SERVER_PORT 80
#define SERVER_SSL_PORT 443

#define FLASH_READ_SIZE 2052
#define HTML_FILE_SIZE 2052

#define URLSize 10

typedef enum Result_Resp {
    RespFail = 0,
    RespSuc,
} Result_Resp;

typedef enum ProtocolType {
    GET = 0,
    POST,
} ProtocolType;

typedef enum _ParmType {
    SWITCH_STATUS = 0,
    INFOMATION,
    WIFI,
    SCAN,
	REBOOT,
    DEEP_SLEEP,
    LIGHT_STATUS,
    CONNECT_STATUS,
    USER_BIN
} ParmType;

typedef struct URL_Frame {
    enum ProtocolType Type;
    char pSelect[URLSize];
    char pCommand[URLSize];
    char pFilename[URLSize];
} URL_Frame;

typedef struct _rst_parm {
    ParmType parmtype;
    struct espconn *pespconn;
} rst_parm;

void webconfig_init(char *ssidname, config_cd_t u_config_cd);
void webserver_recon(void *arg, sint8 err);
void webserver_listen(void *arg);
void webserver_sent(void *arg);
void webserver_discon(void *arg);
void webserver_recv(void *arg, char *pusrdata, unsigned short length);
void data_send(void *arg, bool responseOK, char *psend);
void parse_url(char *precv, URL_Frame *purl_frame);
void webconfig_wifi_connect(char *psend);
void webconfig_ota_url(char *psend);


#define HTML_INDEX  "<!DOCTYPE html>\n"\
                    "<html>\n"\
                    "<head>\n"\
                    "\t<meta charset=\"utf-8\">\n"\
                    "\t<title>OTA</title>\n"\
                    "\t<link rel=\"stylesheet\" href=\"https://cdn.staticfile.org/twitter-bootstrap/3.3.7/css/bootstrap.min.css\">\n"\
                    "\t<style type=\"text/css\">\n"\
                    "\t.box {\n"\
                    "\t\tposition: absolute;\n"\
                    "\t\ttop: 50%;\n"\
                    "\t\tleft: 50%;"\
                    "\t\tmargin: -150px 0 0 -150px;\n"\
                    "\t\twidth: 300px;\n"\
                    "\t\theight: 300px;\n"\
                    "\t}\n"\
                    "\t</style>\n"\
                    "</head>\n"\
                    "<body>\n"\
                    "\t<div class=\"box\">\n"\
                    "\t\t<h1 class=\"text-center\">网页配置</h1>\n"\
                    "\t\t<br>\n"\
                    "\t\t<a href=\"wifi\" class=\"btn btn-success\" style=\"width: 300px;\">WIFI配置</a>\n"\
                    "\t\t</br>\n"\
                    "\t\t</br>\n"\
                    "\t\t<a href=\"ota\" class=\"btn btn-primary\" style=\"width: 300px;\">OTA升级</a>\n"\
                    "\t</div>\n"\
                    "</body>\n"\
                    "</html>"


#define OTA_HTML_INDEX  "<!DOCTYPE html>\n"\
                    "<html>\n"\
                    "<head>\n"\
                    "\t<meta charset=\"utf-8\">\n"\
                    "\t<title>OTA</title>\n"\
                    "\t<link rel=\"stylesheet\" href=\"https://cdn.staticfile.org/twitter-bootstrap/3.3.7/css/bootstrap.min.css\">\n"\
                    "\t<style type=\"text/css\">\n"\
                    "\t.box {\n"\
                    "\t\tposition: absolute;\n"\
                    "\t\ttop: 50%;\n"\
                    "\t\tleft: 50%;"\
                    "\t\tmargin: -150px 0 0 -150px;\n"\
                    "\t\twidth: 300px;\n"\
                    "\t\theight: 300px;\n"\
                    "\t}\n"\
                    "\t</style>\n"\
                    "</head>\n"\
                    "<body>\n"\
                    "\t<div class=\"box\">\n"\
                    "\t\t<form action=\"ota\" method=\"post\">\n"\
                    "\t\t\t<div class=\"form-group\">\n"\
                    "\t\t\t\t<h1 class=\"text-center\">ESP OTA 升级</h1>\n"\
                    "\t\t\t\t<input type=\"text\" class=\"form-control\" id=\"url\" name=\"otaurl\" placeholder=\"http://yourdomain.com:9001/ota/\">\n"\
                    "\t\t\t</div>\n"\
                    "\t\t\t<button type=\"submit\" name=\"submitOK\" class=\"btn btn-primary\" style=\"width: 300px;\">开始升级</button>\n"\
                    "\t\t</form>"\
                    "\t</div>\n"\
                    "</body>\n"\
                    "</html>"

#define WIFI_HTML_INDEX  "<!DOCTYPE html>\n"\
                    "<html>\n"\
                    "<head>\n"\
                    "\t<meta charset=\"utf-8\">\n"\
                    "\t<title>OTA</title>\n"\
                    "\t<link rel=\"stylesheet\" href=\"https://cdn.staticfile.org/twitter-bootstrap/3.3.7/css/bootstrap.min.css\">\n"\
                    "\t<style type=\"text/css\">\n"\
                    "\t.box {\n"\
                    "\t\tposition: absolute;\n"\
                    "\t\ttop: 50%;\n"\
                    "\t\tleft: 50%;"\
                    "\t\tmargin: -150px 0 0 -150px;\n"\
                    "\t\twidth: 300px;\n"\
                    "\t\theight: 300px;\n"\
                    "\t}\n"\
                    "\t</style>\n"\
                    "</head>\n"\
                    "<body>\n"\
                    "\t<div class=\"box\">\n"\
                    "\t\t<form action=\"wifi\" method=\"post\">\n"\
                    "\t\t\t<div class=\"form-group\">\n"\
                    "\t\t\t<br>\n"\
                    "\t\t\t\t<h1 class=\"text-center\">WIFI配置</h1>\n"\
                    "\t\t\t\t<input type=\"text\" class=\"form-control\" id=\"ssid\" name=\"ssid\" placeholder=\"WIFI名称\">\n"\
                    "\t\t\t<br>\n"\
                    "\t\t\t\t<input type=\"text\" class=\"form-control\" id=\"password\" name=\"password\" placeholder=\"WIFI密码\">\n"\
                    "\t\t\t</div>\n"\
                    "\t\t\t<button type=\"submit\" name=\"submitOK\" class=\"btn btn-success\" style=\"width: 300px;\">确定</button>\n"\
                    "\t\t</form>"\
                    "\t</div>\n"\
                    "</body>\n"\
                    "</html>"

#endif
