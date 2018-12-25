#include <fstream>
#include "timemac.h"
//构造函数
TimeMachine::TimeMachine()
{
    //从文件中获取上一次程序结束时的现实时间和虚拟时间
    __time64_t preRtime;
    __time64_t preVtime;
    std::ifstream in_fstream;
    in_fstream.open( time_path , std::ios::binary | std::ios::in );
    in_fstream.read( (char*)&preRtime , sizeof(__time64_t) );
    in_fstream.read( (char*)&preVtime , sizeof(__time64_t) );
    in_fstream.close();
    //求得当前的现实时间和虚拟时间
    _time64(&Rtime);
    Vtime = preVtime + (Rtime - preRtime);

    //设置初始时间流逝速度
    eRate = NORMAL;
}

TimeMachine::~TimeMachine()
{
    std::ofstream Info_ostream;
    Info_ostream.open( time_path , std::ios::binary | std::ios::out );
    Info_ostream.write( (char*)&Rtime , sizeof(__time64_t) );
    Info_ostream.write( (char*)&Vtime , sizeof(__time64_t) );
    Info_ostream.close();
}

void TimeMachine::SetElapsingRate(ElapsingRate erate)
{
    TimeMachine::Now();
    eRate = erate;
}

__time64_t  TimeMachine::Now()
{
    __time64_t preRtime = Rtime;
    _time64(&Rtime);
    if( eRate == NORMAL )
    {
        Vtime = Vtime + Rtime - preRtime;
    }
    else
    {
        Vtime = Vtime + 3600*(Rtime-preRtime);
    }
    return Vtime;
}

char* TimeMachine::ShowNow()
{
    TimeMachine::Now();
    struct tm * ptm;
    ptm = _gmtime64(&Vtime);
    return asctime(ptm);
}

char* TimeMachine::ConvertDate(__time64_t time)
 {

     struct tm * ptm;
     ptm = _gmtime64(&time);
     char* pp = asctime(ptm);

     int i;
     for(i = 0; pp[i]!='\n' ; i++);
     pp[i] = NULL;

     return pp;

 }
