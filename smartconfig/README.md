# ESP8266 Smrtconfig 驱动

ESP8266 NONOS SDK Smrtconfig 驱动，可存储多个WIFI信息，并自动尝试连接
* 设置WIFI连接与断开回调：`set_wifistate_cb(connect_cb, disconnect_cb)`
* 开始Smrtconfig：`start_smartconfig(smartconfig_cd)`