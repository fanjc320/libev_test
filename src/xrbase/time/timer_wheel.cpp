
#include "pch.h"
#include <string.h>
#include <log/LogMgr.h>
#include "timer_wheel.h"
#include "shm/shm_type.h"
#include "shm/shm_mgr.h"
#include "time/time_func_x.h"

namespace timer_wheel
{
    const uint32_t TVN_BITS = 6;
    const uint32_t  TVR_BITS = 8;
    const uint32_t  TVN_SIZE = (1 << TVN_BITS);
    const uint32_t  TVR_SIZE = (1 << TVR_BITS);
    const uint32_t  TVN_MASK = (TVN_SIZE - 1);
    const uint32_t  TVR_MASK = (TVR_SIZE - 1);

    const uint32_t  TV1_MAX = TVR_SIZE;
    const uint32_t  TV2_MAX = (1 << (TVR_BITS + 1 * TVN_BITS));
    const uint32_t  TV3_MAX = (1 << (TVR_BITS + 2 * TVN_BITS));
    const uint32_t  TV4_MAX = (1 << (TVR_BITS + 3 * TVN_BITS));

#define TV1_IDX(tick) ((tick)&TVR_MASK)
#define TV2_IDX(tick) (((tick) >> TVR_BITS) & TVN_MASK)
#define TV3_IDX(tick) (((tick) >> (TVR_BITS + 1 * TVN_BITS)) & TVN_MASK)
#define TV4_IDX(tick) (((tick) >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK)
#define TV5_IDX(tick) (((tick) >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK)

    struct TIMER_MGR;

    static TIMER_MGR* timer_mgr = nullptr;

    static shm::head_mid_node* find_handle_list(int64_t expires);

    struct TIMER_VECTOR
    {
        shm::head_mid_node vec[TVN_SIZE];
    };

    struct TIMER_VECTOR_ROOT
    {
        shm::head_mid_node vec[TVR_SIZE];
    };

    struct TIMER_MGR
    {
        int64_t offset;   // 起服时系统时间和配置时间的差异(秒)
        int64_t cur_tick; // 当前跑到哪个tick

        int64_t start_local_sec; // 系统启动时对应的秒数
        int64_t time_local_sec;

        int64_t time_sec;
        int64_t time_milisec;

        TIMER_VECTOR_ROOT tv1;
        TIMER_VECTOR tv2;
        TIMER_VECTOR tv3;
        TIMER_VECTOR tv4;
        TIMER_VECTOR tv5;

        FUNC_TIMEOUT timeout_funcs[TIMER_TYPE_MAX + 1];
    };

    int init(bool is_resume)
    {
        int ret = 0;

        init_type_name();

        timer_mgr = (TIMER_MGR*) shm_mgr::init_singleton(SHMT_TIMER_MANAGER, sizeof(TIMER_MGR));
        assert_retval(timer_mgr, -1);

        ret = shm_mgr::init_struct_by_conf(SHMT_TIMER, sizeof(timer_node));
        assert_retval(ret == 0, ret);

        if (!is_resume)
        {
            memset(timer_mgr, 0, sizeof(*timer_mgr));

            timer_mgr->offset = 0;

            time_t start_time = get_start_time();
            if (start_time > 0)
            {
                time_t now(time(nullptr));

                timer_mgr->offset = start_time - now;

                LOG_INFO("time offset:%lld, start:%lld, now:%lld.", timer_mgr->offset, start_time, now);
            }

            timer_mgr->start_local_sec = get_curr_time();
            timer_mgr->time_local_sec = get_curr_time();
            timer_mgr->time_milisec = get_tick_count() % 1000;

            timer_mgr->cur_tick = TIME_MS2TICK(timer_mgr->time_milisec);

            timer_mgr->time_sec = timer_mgr->time_local_sec + timer_mgr->offset;
        }

        return 0;
    }

    bool reload()
    {
        LOG_INFO("time cfg reload");

        timer_mgr->offset = 0;

        // 重新计算时间偏移
        time_t start_time = get_start_time();
        if (start_time > 0)
        {
            time_t now(time(nullptr));

            timer_mgr->offset = start_time - now;

            LOG_INFO("reload time offset:%lld, start:%lld, now:%lld.", timer_mgr->offset, start_time, now);
        }

        timer_mgr->time_sec = timer_mgr->time_local_sec + timer_mgr->offset;
        return true;
    }

    int64_t get_cur_ticks()
    {
        return timer_mgr->cur_tick;
    }

    static void cascade(shm::head_mid_node* src)
    {
        uint64_t handle_mid = src->head_mid;
        while (handle_mid != 0)
        {
            timer_node* handle = (timer_node*) shm_mgr::mid2ptr(handle_mid);
            assert_break(handle);

            uint64_t next_handle_mid = handle->next_mid;

            shm::head_mid_node* dst = find_handle_list(handle->expires);
            handle->lst_offset = (int64_t) dst - (int64_t) timer_mgr;
            shm::add_tail(dst, handle_mid);

            handle_mid = next_handle_mid;
        }

        src->head_mid = 0;
        src->tail_mid = 0;
    }

    static void internal_run_timer()
    {
        int tv1_idx = TV1_IDX(timer_mgr->cur_tick);

        do
        {
            if (tv1_idx != 0)
                break;

            int tv2_idx = TV2_IDX(timer_mgr->cur_tick);
            cascade(&timer_mgr->tv2.vec[tv2_idx]);
            if (tv2_idx != 0)
                break;

            int tv3_idx = TV3_IDX(timer_mgr->cur_tick);
            cascade(&timer_mgr->tv3.vec[tv3_idx]);
            if (tv3_idx != 0)
                break;

            int tv4_idx = TV4_IDX(timer_mgr->cur_tick);
            cascade(&timer_mgr->tv4.vec[tv4_idx]);
            if (tv4_idx != 0)
                break;

            int tv5_idx = TV5_IDX(timer_mgr->cur_tick);
            cascade(&timer_mgr->tv5.vec[tv5_idx]);

        } while (0);

        shm::head_mid_node* head = &timer_mgr->tv1.vec[tv1_idx];

        while (head->head_mid != 0)
        {
            uint64_t handle_mid = head->head_mid;

            timer_node* handle = (timer_node*) shm_mgr::mid2ptr(handle_mid);
            assert_continue(handle);

            // 扫到一个"running状态"的handle, 说明是进程在callback阶段core掉了
            if (handle->is_running)
            {
                LOG_ERROR("timer handler crashed at last time, type:%s", type2name(handle->timer_type));
                if (handle->repeats <= 0 && !handle->is_forever)
                {
                    del_node(head, handle_mid);
                    shm_mgr::free_x(handle_mid);
                    continue;
                }
            }

            handle->is_running = 1;
            handle->repeats--;

            timer_mgr->timeout_funcs[handle->timer_type](handle_mid, handle->data, handle->data_len);

            handle->is_running = 0;

            if ((handle->repeats <= 0) && !handle->is_forever)
            {
                del_node(head, handle_mid);
                shm_mgr::free_x(handle_mid);
                continue;
            }

            handle->expires = get_cur_ticks() + handle->interval;
            shm::head_mid_node* list = find_handle_list(handle->expires);
            handle->lst_offset = (int64_t) list - (int64_t) timer_mgr;
            del_node(head, handle_mid);
            add_tail(list, handle_mid);
        }
    }

    inline bool ns_is_current_run_end()
    {
        int tv1_idx = TV1_IDX(timer_mgr->cur_tick);

        shm::head_mid_node* head = &timer_mgr->tv1.vec[tv1_idx];
        if (head->head_mid != 0)
        {
            return false;
        }

        return true;
    }

    static inline void ns_set_time()
    {
        timer_mgr->time_local_sec = set_curr_time();
        timer_mgr->time_milisec = set_tick_count() % 1000;
        timer_mgr->time_sec = timer_mgr->time_local_sec + timer_mgr->offset;
    }

    static inline int64_t ns_realtime_tick()
    {
        // tick应该一直用当前时间跑,这样才能保证时间轴
        return TIME_MS2TICK((int64_t) (timer_mgr->time_local_sec - timer_mgr->start_local_sec) * (int64_t) 1000
            + (int64_t) (timer_mgr->time_milisec));
    }

    void main_loop()
    {
        ns_set_time();
        int64_t tick_num = ns_realtime_tick() - timer_mgr->cur_tick;

        while (tick_num > 0)
        {
            for (int64_t i = 0; i < tick_num; ++i)
            {
                internal_run_timer();

                ++timer_mgr->cur_tick;
            }
            ns_set_time();
            tick_num = ns_realtime_tick() - timer_mgr->cur_tick;
        }
    }

    // (时间轮算法)根据新加timer的超时时间找到合适的链表挂上去(FIFO)
    static shm::head_mid_node* find_handle_list(int64_t expires)
    {
        uint64_t idx = expires - timer_mgr->cur_tick;

        if (idx < TV1_MAX)
        {
            return &timer_mgr->tv1.vec[TV1_IDX(expires)];
        }
        else if (idx < TV2_MAX)
        {
            return &timer_mgr->tv2.vec[TV2_IDX(expires)];
        }
        else if (idx < TV3_MAX)
        {
            return &timer_mgr->tv3.vec[TV3_IDX(expires)];
        }
        else if (idx < TV4_MAX)
        {
            return &timer_mgr->tv4.vec[TV4_IDX(expires)];
        }
        else
        {
            if (idx > UINT32_MAX)
            {
                idx = UINT32_MAX;
                expires = idx + timer_mgr->cur_tick;
            }
            return &timer_mgr->tv5.vec[TV5_IDX(expires)];
        }
    }

    uint64_t add(uint32_t firstcall_interval, uint32_t repeat_interval, uint32_t repeat_num, uint16_t timer_type, const void* cb_data,
        size_t cb_data_len, bool to_head)
    {
        assert_retval((uint32_t) cb_data_len <= MAX_TIMER_CB_DATA_LEN, UINT64_MAX);
        assert_retval(timer_type > 0 && timer_type < TIMER_TYPE_MAX, UINT64_MAX);

        if ((repeat_interval == 0) && (repeat_num == REPEAT_FOREVER))
        {
            assert_retval(0, UINT64_MAX);
        }

        assert_retval(timer_mgr->timeout_funcs[timer_type], UINT64_MAX);

        timer_node* handle = nullptr;
        uint64_t handle_mid = shm_mgr::alloc_x(SHMT_TIMER, (void**) &handle);
        assert_retval(handle, UINT64_MAX);

        handle->expires = get_cur_ticks() + firstcall_interval;
        handle->interval = repeat_interval;
        handle->timer_type = timer_type;
        handle->repeats = repeat_num;
        handle->is_forever = (repeat_num == REPEAT_FOREVER ? 1 : 0);
        handle->is_running = 0;
        handle->data_len = (uint16_t) cb_data_len;
        if (cb_data_len > 0)
        {
            memcpy(handle->data, cb_data, cb_data_len);
        }

        shm::head_mid_node* list = find_handle_list(handle->expires);
        handle->lst_offset = (int64_t) list - (int64_t) timer_mgr;
        if (to_head)
        {
            add_head(list, handle_mid);
        }
        else
        {
            add_tail(list, handle_mid);
        }

        return handle_mid;
    }

    uint64_t add_single(uint32_t interval, uint16_t timer_type, const void* cb_data, size_t cb_data_len)
    {
        return add(interval, 0, 1, timer_type, cb_data, cb_data_len, false);
    }

    uint64_t add_single_head(uint32_t interval, uint16_t timer_type, const void* cb_data, size_t cb_data_len)
    {
        return add(interval, 0, 1, timer_type, cb_data, cb_data_len, true);
    }

    uint64_t add_forever(uint32_t firstcall_interval, uint32_t repeat_interval, uint16_t timer_type, const void* cb_data, size_t cb_data_len)
    {
        return add(firstcall_interval, repeat_interval, REPEAT_FOREVER, timer_type, cb_data, cb_data_len, false);
    }

    int del(uint64_t timer_mid)
    {
        if (timer_mid == 0)
            return 0;

        timer_node* handle = (timer_node*) shm_mgr::mid2ptr(timer_mid);
        if (handle == nullptr)
        {
            return -1;
        }

        if (handle->is_running)
        {
            handle->repeats = 0;
            handle->is_forever = 0;
            return 0;
        }

        LOG_DEBUG("timer:%llu %s del.", timer_mid, type2name(handle->timer_type));

        assert_retval(handle->lst_offset > 0, -1);
        assert_retval(handle->lst_offset < (int) sizeof(*timer_mgr), -1);
        shm::head_mid_node* list = (shm::head_mid_node*) ((int64_t) timer_mgr + handle->lst_offset);
        shm::del_node(list, timer_mid);
        shm_mgr::free_x(timer_mid);

        return 0;
    }

    void* get_cbdata(uint64_t timer_mid)
    {
        timer_node* handle = (timer_node*) shm_mgr::mid2ptr(timer_mid);
        if (nullptr == handle)
        {
            return nullptr;
        }

        return handle->data;
    }

    bool is_finish(uint64_t timer_id)
    {
        timer_node* handle = (timer_node*) shm_mgr::mid2ptr(timer_id);
        if (nullptr == handle)
        {
            return true;
        }

        return handle->repeats == 0 && handle->is_forever == 0;
    }

    int64_t get_remain_tick(uint64_t timer_id)
    {
        timer_node* handle = (timer_node*) shm_mgr::mid2ptr(timer_id);
        if (nullptr == handle)
            return 0;

        int64_t timenow = get_cur_ticks();
        if (timenow > handle->expires)
            return 0;

        return handle->expires - timenow;
    }

    int set_timer_callback(uint16_t timer_type, FUNC_TIMEOUT func)
    {
        assert_retval(timer_type < TIMER_TYPE_MAX, -1);
        assert_retval(func, -1);
        assert(timer_mgr->timeout_funcs[timer_type] != nullptr);

        timer_mgr->timeout_funcs[timer_type] = func;
        return 0;
    }
}