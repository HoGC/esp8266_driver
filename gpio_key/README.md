# ESP8266 按键与GPIO 驱动

ESP8266 NONOS SDK 按键 GPIO 驱动，以适配D1 mini引脚
* 设置按键数量：`set_key_num(2)`
* 按键D5(GPIO14)长按短按按键回调：`key_add(D5,long_press,short_press)`
* 按键D6(GPIO12)按下松开按键回调：`status_key_add(D6,key_down,key_up)`