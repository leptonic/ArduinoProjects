>>>DoorBase son 门禁系统<<< 未完全部署 原因是wifi模块不够稳定，手机连接经常出错。

根据激光传感器和wifi模块来判断是否开门的系统
============================================================================================
HARDWARE
-------------------------
wifi用来创建局域网，检测进入网内的手机mac地址
lazer tx 发射激光
lazer rx 检测激光
ring led 提示走廊灯
beep 提示
relay 控制门锁
misc
小米智能插座
==================================================

规则
本系统有小米智能插座提供电源控制


0.设定wifi 为wpa2加密方式，并且把注册手机进入网内

1.定期检测 ，若有手机且符合mac地址的进入网内
1.1 亮欢迎换色灯
2.且激光中断
3.亮白灯
4.闭合继电器，开门