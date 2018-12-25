#ifndef TIMEMAC_H
#define TIMEMAC_H
#include <ctime>
#include "common.h"

//时间规定：
//    0、本程序使用UTC时间，不适用本地时间。
//    1、程序未打开的时候，时间以正常的时间流逝速度流逝
//    2、程序打开的时候，从文件中读取上次程序关闭的时间点（虚拟时间与现实时间），根据UTC时间算出当前的虚拟时间。
//    3、此时程序中有两个时间，一个是虚拟时间，一个是现实时间。
//    4、初始的虚拟时间流逝速度为NORMAL
//    5、第一次调用NOW函数的时候，根据现实时间UTC时间算出过了多少秒，然后根据时间流逝速度算出虚拟时间，（也就是说，每次调用NOW函数都会改变虚拟时间与现实时间的值）
//    6、虚拟时间的起始时间为2017.1.1。
//    7、普通的时间流逝速度和正常时间一致，快速的时间流逝速度为现实流逝速度的3600倍。
//    8、因图书馆管理系统对时间的精确读要求不是很高，本程序的时间计量的最小单位为秒。

/* 时间流逝速度 */
enum ElapsingRate { NORMAL, FAST };

/* TimeMachine: 时间机器 */
class TimeMachine
{
public:
    TimeMachine();
    ~TimeMachine();


    //接口
    void        SetElapsingRate(enum ElapsingRate erate);  // 设置时间流逝速度
    __time64_t  Now();                                     // 获取当前时间，从00:00 , Jan 1, 1970 UTC 开始到虚拟时间的秒数。
    char*       ShowNow();                                 // 返回当前时间的字符串表示
    static char*       ConvertDate(__time64_t time);

private:
    __time64_t Vtime;    //虚拟时间
    __time64_t Rtime;    //正常时间
    ElapsingRate eRate;  //时间流逝速度
    const char* time_path = "Database\\time.bin";
};

#endif




/*

    TimeMachine tm;

    tm.SetElapsingRate(NORMAL);

    tm.Now();

    tm.ShowNow();

    tm.SetElapsingRate(FAST);

*/

