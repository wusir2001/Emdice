# EmDiceBot
本项目基于[酷Q C++ SDK](https://github.com/CoolQ/cqsdk-vc/)开发，用于酷Q for Docker，在其他平台上使用或许需要对源代码进行部分修改；

本文件包含部分来自[此贴](https://cqp.cc/t/26287/)的代码，在此一并感谢。

在SDK基础上的说明
--------
您可以创建CQTool对象，调用CQTool.GetStrangerInfo和GetGroupMemberInfo成员函数取代SDK自带的粗糙接口，具体实现细节参看文件。

使用说明
--------
命令可通过向bot发送.help查看。
在使用前请确认服务器的时区和时间与您所在地一致。
