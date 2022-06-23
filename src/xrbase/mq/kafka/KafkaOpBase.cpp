#include "pch.h"
#include "rdkafkacpp.h"

#include "KafkaCacheBase.h"
#include "time/time_func_x.h"

void KafkaOpBase::OnClose(const string& errstr)
{
    //一个小时内的多次报警短信提醒
    if ((_tmErrorPre + 3600) > get_curr_time())
    {
        _tmErrorPre = get_curr_time();
    }
    else
        _tmErrorPre = get_curr_time();
}
