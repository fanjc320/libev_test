
#pragma once

#include "time/timer_wheel_type.h"
#include "shm/shm_mid_list.h"

typedef void (*FUNC_TIMEOUT)(uint64_t timer_mid, void* cb_data, size_t cb_data_len);
namespace timer_wheel
{
    const uint32_t REPEAT_FOREVER = 0;
    const uint32_t TICK_PER_SECOND = 50;   // 1秒分为50个tick,1tick等于20ms
    const uint32_t MS_PER_TICK = 20;        //(1000/TICK_PER_SECOND)

    #define TIME_SEC2MS(s)     ((int64_t)(s)*1000)
    #define TIME_SEC2TICK(s)   ((int64_t)(s)*TICK_PER_SECOND)
    #define TIME_MS2TICK(ms)   (((int64_t)(ms)) / MS_PER_TICK)
    #define TIME_TICK2MS(tick) (((int64_t)(tick)) * MS_PER_TICK)

    const uint32_t MAX_TIMER_CB_DATA_LEN = 256;  // timer实例中回调参数的最大长度

    struct timer_node : public shm::list_mid_node
    {
        int64_t expires = 0;     // 超时时间点(tick)
        int64_t lst_offset = 0;  // 当前timer handle对象所在list相对于timer_mgr的偏移
        int32_t interval = 0;    // 间隔时间
        int32_t repeats = 0;     // 执行次数
        uint16_t is_forever = 0; // 永久执行
        uint16_t is_running = 0; // 是否正在被处理，防止超时处理中删除自己
        uint16_t timer_type = 0; // 回调函数id
        uint16_t data_len = 0;   // 附加的数据
        char     data[MAX_TIMER_CB_DATA_LEN] = {};
    };

    int     init(bool is_resume);// 初始化
    bool    reload();// 重新加载配置
    void    main_loop();
    int64_t get_cur_ticks();

    uint64_t add(uint32_t firstcall_interval, // timer第一次被唤醒的延迟时间(单位:tick)
                    uint32_t repeat_interval, // timer第二次之后(含第二次)的超时间隔(单位:tick)
                    uint32_t repeat_num, // 重复次数，无限调用使用REPEAT_FOREVER
                    uint16_t timer_type, const void* cb_data, size_t cb_data_len, bool to_head = false);

    uint64_t    add_single(uint32_t interval, uint16_t timer_type, const void* cb_data, size_t cb_data_len);//只触发一次
    uint64_t    add_single_head(uint32_t interval, uint16_t timer_type, 
                        const void* cb_data, size_t cb_data_len);// 只触发一次的简化版本
    uint64_t    add_forever(uint32_t firstcall_interval, uint32_t repeat_interval, 
                        uint16_t timer_type, const void* cb_data, size_t cb_data_len);// 永久timer
    int         set_timer_callback(uint16_t timer_type, FUNC_TIMEOUT func);// 设置回调

    int         del(uint64_t timer_mid);
    void*       get_cbdata(uint64_t timer_mid);
    bool        is_finish(uint64_t timer_mid);
    int64_t     get_remain_tick(uint64_t timer_id);

    inline int32_t get_tick_per_sec(){ return TICK_PER_SECOND; }
}
