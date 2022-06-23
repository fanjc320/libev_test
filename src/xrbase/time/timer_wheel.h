
#pragma once

#include "time/timer_wheel_type.h"
#include "shm/shm_mid_list.h"

typedef void (*FUNC_TIMEOUT)(uint64_t timer_mid, void* cb_data, size_t cb_data_len);
namespace timer_wheel
{
    const uint32_t REPEAT_FOREVER = 0;
    const uint32_t TICK_PER_SECOND = 50;   // 1���Ϊ50��tick,1tick����20ms
    const uint32_t MS_PER_TICK = 20;        //(1000/TICK_PER_SECOND)

    #define TIME_SEC2MS(s)     ((int64_t)(s)*1000)
    #define TIME_SEC2TICK(s)   ((int64_t)(s)*TICK_PER_SECOND)
    #define TIME_MS2TICK(ms)   (((int64_t)(ms)) / MS_PER_TICK)
    #define TIME_TICK2MS(tick) (((int64_t)(tick)) * MS_PER_TICK)

    const uint32_t MAX_TIMER_CB_DATA_LEN = 256;  // timerʵ���лص���������󳤶�

    struct timer_node : public shm::list_mid_node
    {
        int64_t expires = 0;     // ��ʱʱ���(tick)
        int64_t lst_offset = 0;  // ��ǰtimer handle��������list�����timer_mgr��ƫ��
        int32_t interval = 0;    // ���ʱ��
        int32_t repeats = 0;     // ִ�д���
        uint16_t is_forever = 0; // ����ִ��
        uint16_t is_running = 0; // �Ƿ����ڱ�������ֹ��ʱ������ɾ���Լ�
        uint16_t timer_type = 0; // �ص�����id
        uint16_t data_len = 0;   // ���ӵ�����
        char     data[MAX_TIMER_CB_DATA_LEN] = {};
    };

    int     init(bool is_resume);// ��ʼ��
    bool    reload();// ���¼�������
    void    main_loop();
    int64_t get_cur_ticks();

    uint64_t add(uint32_t firstcall_interval, // timer��һ�α����ѵ��ӳ�ʱ��(��λ:tick)
                    uint32_t repeat_interval, // timer�ڶ���֮��(���ڶ���)�ĳ�ʱ���(��λ:tick)
                    uint32_t repeat_num, // �ظ����������޵���ʹ��REPEAT_FOREVER
                    uint16_t timer_type, const void* cb_data, size_t cb_data_len, bool to_head = false);

    uint64_t    add_single(uint32_t interval, uint16_t timer_type, const void* cb_data, size_t cb_data_len);//ֻ����һ��
    uint64_t    add_single_head(uint32_t interval, uint16_t timer_type, 
                        const void* cb_data, size_t cb_data_len);// ֻ����һ�εļ򻯰汾
    uint64_t    add_forever(uint32_t firstcall_interval, uint32_t repeat_interval, 
                        uint16_t timer_type, const void* cb_data, size_t cb_data_len);// ����timer
    int         set_timer_callback(uint16_t timer_type, FUNC_TIMEOUT func);// ���ûص�

    int         del(uint64_t timer_mid);
    void*       get_cbdata(uint64_t timer_mid);
    bool        is_finish(uint64_t timer_mid);
    int64_t     get_remain_tick(uint64_t timer_id);

    inline int32_t get_tick_per_sec(){ return TICK_PER_SECOND; }
}
