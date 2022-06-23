
#include "pch.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "shm/shm_conf.h"
#include "shm/shm_def.h"
#include "shm/shm_mgr.h"
#include "shm/shm_type.h"
#include "shm/shm_x.h"
#include "log/LogMgr.h"
#include "time/time_func_x.h"
#include "string/str_format.h"
#include "platform/file_func.h"
#include "math/math_basic.h"
#include "shm/shm_struct_def.h"

#ifndef _MSC_VER
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#endif // _MSC_VER

namespace shm_mgr
{
    static SHM_FILE_HANDLE g_shm_handle;
    const  uint32_t UNIT_SERIAL_MASK = 0x3fff;
    static SHM_MGR* g_shm_mgr = nullptr;
    static char g_shm_file[1024] = {};

    int32_t     _shm_try_to_unmap_node(SHM_STRUCT_POOL& pool, int32_t node_index);
    int32_t     _shm_try_to_move_backward_node(SHM_STRUCT_POOL& pool);
    int32_t     _shm_try_to_map_node(SHM_STRUCT_POOL& pool, int32_t node_index);
    std::string _shm_get_shm_name(int32_t shm_type, int32_t index, const char* sh_name = nullptr);
    int         _init_struct(int32_t shm_type, int32_t item_size, int32_t item_base_num,
                        FUNC_STRUCT_RESUME resume_func = nullptr, EStructGrowType growth_mode = EStructGrowType::DEFAULT,
                        int64_t max_item_num = -1, bool pool0_alloc = true);
    BOOL is_resume()
    {
        return g_shm_mgr ? g_shm_mgr->is_resume : FALSE;
    }

    const char* get_shm_name()
    {
        return g_shm_mgr ? g_shm_mgr->filename : g_shm_file;
    }

    SHM_MGR* get_shm_mgr()
    {
        return g_shm_mgr;
    }

    int start_release(const char* name)
    {
        delete_file_all(name);
        return 0;
    }

    int shm_init_start(const char* name)
    {
        uint64_t size = ALIGN8(sizeof(SHM_MGR));

        //需要对旧的做一次释放操作
        int ret = start_release(name);
        strncpy_safe(g_shm_file, name, strlen(name));

        //重新生成新的
        g_shm_handle.handle = shm::shm_create_x(name, size, (void**) &g_shm_mgr, g_shm_handle.fhandle);
        CHECK_LOG_RETVAL(g_shm_handle.handle != nullptr, -1);

        memset(g_shm_mgr, 0, sizeof(*g_shm_mgr));
        strncpy_safe(g_shm_mgr->filename, name, strlen(name));

        g_shm_mgr->is_resume = FALSE;
        g_shm_handle.addr = g_shm_mgr;

        for (int32_t i = 0; i < SHM_TYPE_MAX; ++i)
        {
            SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[i];

            pool.growth_type = (int32_t)EStructGrowType::DEFAULT;
            pool.dirty_flag = FALSE;
            pool.shm_type = i;

            pool_init_item_num(pool);
            for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
            {
                file_handle_init(pool.node_array[j]);
            }
        }

        LOG_INFO("shm_mgr base_addr %p shm_name %s is_resume %d",
            g_shm_mgr, get_shm_name(), is_resume());

        return 0;
    }


    int shm_init_resume(const char* name, bool check_resume_freq)
    {
        strncpy_safe(g_shm_file, name, strlen(name));

        int64_t size = ALIGN8(sizeof(SHM_MGR));
        int64_t shm_size = 0;
        g_shm_handle.handle = shm::shm_open_x(name, (void**) &g_shm_mgr, shm_size, g_shm_handle.fhandle);

        CHECK_LOG_RETVAL(g_shm_handle.handle != nullptr, -1);
        CHECK_LOG_RETVAL(shm_size == size, -1);
        strncpy_safe(g_shm_mgr->filename, name, strlen(name));

        time_t tmSec = get_curr_time();
        g_shm_handle.addr = g_shm_mgr;

        // 三分钟内resume第五次了
        if (check_resume_freq && tmSec - g_shm_mgr->last_resume_sec[LAST_START_RECORD_COUNT - 1] < 180)
        {
            LOG_ERROR("RESUME too frequently. use START mode.");
            g_shm_mgr->is_resume = FALSE;
            for (int32_t i = 0; i < SHM_TYPE_MAX; ++i)
            {
                SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[i];
                pool.dirty_flag = FALSE;
                pool.growth_type = (int32_t)EStructGrowType::DEFAULT;
                pool.shm_type = i;
                pool_init_item_num(pool);

                for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
                {
                    if (is_file_handle_empty(pool.node_array[j]))  continue;
                    shm::shm_unlink_x(_shm_get_shm_name(pool.shm_type, j));
                    file_handle_init(pool.node_array[j]);
                }
            }
        }
        else
        {
            for (int32_t i = 0; i < SHM_TYPE_MAX; ++i)
            {
                SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[i];
                if (pool.item_base_num <= 0) continue;
                for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
                {
                    if (is_file_handle_empty(pool.node_array[j])) continue;
                    pool.node_array[j].handle = shm::shm_open_x(_shm_get_shm_name(pool.shm_type, j), (void**) &(pool.node_array[j].addr), shm_size, pool.node_array[j].fhandle);
                    CHECK_LOG_RETVAL(pool.node_array[j].handle != nullptr, -1);
                }
            }

            for (int32_t i = LAST_START_RECORD_COUNT - 1; i > 0; --i)
            {
                g_shm_mgr->last_resume_sec[i] = g_shm_mgr->last_resume_sec[i - 1];
            }

            g_shm_mgr->last_resume_sec[0] = tmSec;
            g_shm_mgr->is_resume = TRUE;
        }

        LOG_INFO("shm_mgr base_addr 0x%16p shm_name %s shm_size %llu is_resume %d", g_shm_mgr, get_shm_name(),
            ALIGN8(sizeof(SHM_MGR)), is_resume());
        return 0;
    }

    int init_shm(const char* name, bool is_resume, bool check_resume_freq)
    {
        int ret = 0;
        if (!is_resume)
        {
            ret = shm_init_start(name);
            CHECK_LOG_RETVAL(ret == 0, ret);
        }
        else
        {
            ret = shm_init_resume(name, check_resume_freq);
            CHECK_LOG_RETVAL(ret == 0, ret);
        }

        return ret;
    }

    int fini_shm()
    {
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, -1);
        int ret = 0;
        bool r = false;
        for (int i = 0; i < SHM_TYPE_MAX; ++i)
        {
            if(!is_exsit_type2name(i)) continue;

            SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[i];
            if (pool.item_base_num <= 0) continue;
            for (int j = 0; j < POOL_NODE_COUNT; ++j)
            {
                if (is_file_handle_valid(pool.node_array[j]))
                    file_handle_init(pool.node_array[j]);
                else
                {
					continue;
                }

                if (!shm::shm_unlink_x(_shm_get_shm_name(pool.shm_type, j)))
                {
                    LOG_ERROR("shm_ctl failed idx %d, %s", j, type2name(i));
                }
            }
        }
        r = shm::shm_unlink_x(get_shm_name());
        CHECK_LOG_RETVAL(r, -1);
        g_shm_mgr = nullptr;
        if(is_file_handle_valid(g_shm_handle))
            file_handle_init(g_shm_handle);
        return ret;
    }

    int stop_shm()
    {
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, -1);

        bool ret = 0;
        for (int i = 0; i < SHM_TYPE_MAX; ++i)
        {
            if (!is_exsit_type2name(i)) continue;
            SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[i];
            if (pool.item_base_num <= 0) continue;
            for (int j = 0; j < POOL_NODE_COUNT; ++j)
            {
                if (!is_file_handle_valid(pool.node_array[j])) continue;

                ret = shm::shm_munmap(pool.node_array[j].addr, pool.pool_real_mem_size);
                CHECK_LOG_ERROR(ret);
                ret = shm::shm_close_handle(pool.node_array[j].handle, pool.node_array[j].fhandle);
                CHECK_LOG_ERROR(ret);
                file_handle_init(pool.node_array[j]);
            }
        }
        if (!is_file_handle_valid(g_shm_handle)) return 0;
        ret = shm::shm_munmap(g_shm_handle.addr, sizeof(SHM_MGR));
        CHECK_LOG_ERROR(ret);
        ret = shm::shm_close_handle(g_shm_handle.handle, g_shm_handle.fhandle);
        g_shm_mgr = nullptr;
        file_handle_init(g_shm_handle);
        return 0;
    }

    int64_t get_growth_mem_size(SHM_STRUCT_POOL& pool, int32_t node_index, uint64_t& pool_node_item_num)
    {
        switch (EStructGrowType(pool.growth_type))
        {
            case EStructGrowType::EXP:
                {
                    pool_node_item_num = (1LL << node_index) * pool.item_base_num;
                    break;
                }
            case EStructGrowType::LINEAR:
                {
                    pool_node_item_num = (1LL + node_index) * pool.item_base_num;
                    break;
                }
            case EStructGrowType::DEFAULT:
            default:
                {
                    pool_node_item_num = pool.item_base_num;
                    break;
                }
        }

        int64_t pool_node_real_size = ALIGN8(SIZE_STRUCT_HEAD + (pool.item_real_data_size * pool_node_item_num));
        return pool_node_real_size;
    }

    int32_t _shm_try_to_map_node(SHM_STRUCT_POOL& pool, int32_t node_index)
    {
        CHECK_LOG_RETVAL(node_index >= 0 && node_index < POOL_NODE_COUNT, -1);
        CHECK_LOG_RETVAL(pool.item_base_num > 0, -1);

        for (; node_index < POOL_NODE_COUNT; ++node_index)
        {
            if (!is_file_handle_valid(pool.node_array[node_index])) break;

            SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[node_index].addr;
            if (pool_node->head_item_offset != 0)
            {
                CHECK_LOG_RETVAL(pool_node->free_item_num > 0, -1);
                LOG_WARNING("shm[%d] pool current_node_index is being moved forward[%d->%d]", pool.shm_type,
                    pool.cur_node_index, node_index);
                pool.cur_node_index = node_index;
                return 0;
            }
        }

        uint64_t pool_node_item_num = 0;
        pool.pool_real_mem_size = get_growth_mem_size(pool, node_index, pool_node_item_num);

        int32_t shmfd = 0;
        shm::HANDLE_T handle = nullptr;

        if (node_index >= POOL_NODE_COUNT) return -1;

        handle = shm::shm_create_x(_shm_get_shm_name(pool.shm_type, node_index), pool.pool_real_mem_size, (&pool.node_array[node_index].addr), shmfd);

        CHECK_LOG_RETVAL(handle != nullptr, -1);
        pool.cur_node_index = node_index;

        SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[node_index].addr;
        pool_node->mapped_item_num = pool_node_item_num;
        pool_node->free_item_num = pool_node_item_num;
        pool_node->head_item_offset = PTR2OFFSET(pool_node, pool_node->lst_struct);
        pool.node_array[node_index].fhandle = shmfd;
        pool.node_array[node_index].handle = handle;

        time_t time_now = time(nullptr);
        pool_node->item_serial = ((uint32_t) time_now & (uint32_t) UNIT_SERIAL_MASK);

        for (int64_t i = 0; i < pool_node->mapped_item_num; ++i)
        {
            SHM_STRUCT* item = (SHM_STRUCT*) OFFSET2PTR(pool_node, pool_node->head_item_offset + (int64_t) pool.item_real_data_size * i);
            item->next_item_offset = pool_node->head_item_offset + (int64_t) pool.item_real_data_size * (i + 1);
            item->item_serial = 0;

            if (i == pool_node->mapped_item_num - 1)  item->next_item_offset = 0;
        }

        pool.item_mapped_num += pool_node->mapped_item_num;
        pool.item_free_num += pool_node->free_item_num;
        pool.dirty_flag = FALSE;

        return 0;
    }

    int32_t _shm_try_to_move_backward_node(SHM_STRUCT_POOL& pool)
    {
        SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[pool.cur_node_index].addr;
        do
        {
            if (pool.cur_node_index <= 0 ) break;
            if (pool_node->free_item_num * 2 >= pool_node->mapped_item_num) break;
            for (int32_t i = pool.cur_node_index - 1; i >= 0; --i)
            {
                if (is_file_handle_valid(pool.node_array[i]))
                {
                    SHM_STRUCT_HEAD* previous_pool_node = (SHM_STRUCT_HEAD*) pool.node_array[i].addr;
                    if (previous_pool_node->free_item_num * 2 < previous_pool_node->mapped_item_num) continue;
                    LOG_INFO("shm[%d] pool current_node_index is being moved backward[%d->%d]", pool.shm_type,
                        pool.cur_node_index, i);
                    pool.cur_node_index = i;
                    break;
                }
            }
        } while (false);

        return 0;
    }

    int32_t _shm_try_to_unmap_node(SHM_STRUCT_POOL& pool, int32_t node_index)
    {
        _shm_try_to_move_backward_node(pool);

        SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[node_index].addr;
        do
        {
            if (pool_node->free_item_num < pool_node->mapped_item_num) break;
            if (node_index <= 0 && node_index == pool.cur_node_index)  break;

            if (node_index != pool.cur_node_index)
            {
                pool.item_mapped_num -= pool_node->mapped_item_num;
                pool.item_free_num -= pool_node->free_item_num;

                shm::shm_munmap(pool.node_array[node_index].addr, pool.pool_real_mem_size);
                shm::shm_close_handle(pool.node_array[node_index].handle, pool.node_array[node_index].fhandle);
                shm::shm_unlink_x(_shm_get_shm_name(pool.shm_type, node_index));

                file_handle_init(pool.node_array[node_index]);
                break;
            }

            int32_t previous_node_index = pool.cur_node_index - 1;
            assert_break(previous_node_index >= 0);

            SHM_STRUCT_HEAD* previous_pool_node = (SHM_STRUCT_HEAD*) pool.node_array[previous_node_index].addr;
            if (is_file_handle_empty(pool.node_array[previous_node_index])
                || previous_pool_node->free_item_num * 2 >= previous_pool_node->mapped_item_num)
            {
                pool.item_mapped_num -= pool_node->mapped_item_num;
                pool.item_free_num -= pool_node->free_item_num;

                shm::shm_munmap(pool.node_array[node_index].addr, pool.pool_real_mem_size);
                shm::shm_close_handle(pool.node_array[node_index].handle, pool.node_array[node_index].fhandle);
                shm::shm_unlink_x(_shm_get_shm_name(pool.shm_type, node_index));
                file_handle_init(pool.node_array[node_index]);

                bool found_current_index = false;
                for (int32_t i = node_index - 1; i >= 0; --i)
                {
                    if (is_file_handle_valid(pool.node_array[i]))
                    {
                        pool.cur_node_index = i;
                        found_current_index = true;
                        break;
                    }
                }

                if (found_current_index == false)
                {
                    int32_t ret = _shm_try_to_map_node(pool, 0);
                    if (ret != 0)
                    {
                        pool.cur_node_index = -1;
                        LOG_ERROR("It is fatal shm dynamic free, ret[%d]!", ret);
                        CHECK_LOG_RETVAL(0, ret);
                    }
                }
            }
        } while (false);

        return 0;
    }

    int32_t main_loop()
    {
        for (int32_t i = 0; i < SHM_TYPE_MAX; ++i)
        {
            SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[i];
            if (pool.dirty_flag == FALSE) continue;
            for (int32_t j = 0; j <= pool.cur_node_index; ++j)
            {
                if (is_file_handle_empty(pool.node_array[j]))  continue;
                _shm_try_to_unmap_node(pool, j);
            }
            pool.dirty_flag = FALSE;
        }

        return 0;
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    int shm_init_struct_start(int32_t shm_type, int32_t item_size, int32_t item_num, EStructGrowType growth_mode, int64_t item_max_num,
        bool pool0_alloc)
    {
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, -1);
        CHECK_LOG_RETVAL(item_size > 0, -1);
        CHECK_LOG_RETVAL(item_num > 0, -1);
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, -1);

        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_type];
        CHECK_LOG_RETVAL(pool.item_base_num <= 0, -1);

        pool.item_data_size = item_size;
        pool.item_real_data_size = ALIGN8(sizeof(SHM_STRUCT) + item_size);
        pool.item_base_num = item_num;
        pool.growth_type = (int32_t)growth_mode;
        pool.item_total_num = item_max_num;
        pool.item_mapped_num = 0;
        pool.item_free_num = 0;

        if (pool0_alloc == false) return 0;
        return _shm_try_to_map_node(pool, 0);
    }

    int shm_init_struct_resume(int shm_type, FUNC_STRUCT_RESUME resume_func)
    {
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, -1);

        if (resume_func == nullptr) return 0;
        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_type];
        //下面要开始遍历所有pool_node了
        for (int32_t i = 0; i < POOL_NODE_COUNT; ++i)
        {
            if (!is_file_handle_valid(pool.node_array[i]))  continue;

            SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[i].addr;
            for (int64_t j = 0; j < pool_node->mapped_item_num; ++j)
            {
                SHM_STRUCT* item = (SHM_STRUCT*) ((int64_t) pool_node->lst_struct + (int64_t) pool.item_real_data_size * j);
                if (item->item_serial != 0)  resume_func(item->item_data);
            }
        }

        return 0;
    }

    int _init_struct(int32_t shm_type, int32_t item_size, int32_t item_base_num, FUNC_STRUCT_RESUME resume_func, EStructGrowType growth_mode,
        int64_t item_max_num, bool pool0_alloc)
    {
        if (!is_resume())
        {
            return shm_init_struct_start(shm_type, item_size, item_base_num, growth_mode, item_max_num, pool0_alloc);
        }
        else
        {
            return shm_init_struct_resume(shm_type, resume_func);
        }

        return 0;
    }

    int init_struct_by_conf(int32_t shm_type, int32_t item_size, FUNC_STRUCT_RESUME resume_func, bool pool0_alloc)
    {
        const char* shm_name = type2name(shm_type);
        std::string growth_mode = std::string(shm_name) + "_GROWTH";
        std::string max_num = std::string(shm_name) + "_MAX";

        int32_t item_base_num = shm_conf_get_int(shm_name);
        int32_t shm_pool_growth_mode = shm_conf_get_int(growth_mode.c_str());
        int64_t item_max_num = shm_conf_get_long(max_num.c_str());

        EStructGrowType  growth_mode1;
        if (shm_pool_growth_mode == -1)
        {
            growth_mode1 = EStructGrowType::EXP;
        }
        else
            growth_mode1 = (EStructGrowType) (shm_pool_growth_mode);

        return (item_base_num <= 0) ? 0
            : _init_struct(shm_type, item_size, item_base_num, resume_func, growth_mode1,
                item_max_num, pool0_alloc);
    }

    void* init_singleton(int32_t shm_type, int32_t item_size)
    {
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, nullptr);

        void* ptr = nullptr;

        if (is_resume())
        {
            ptr = idx2ptr(shm_type, 0);
        }
        else
        {
            int32_t ret = _init_struct(shm_type, item_size, 1, nullptr, EStructGrowType::EXP, 1, true);
            CHECK_LOG_RETVAL(ret == 0, nullptr);

            uint64_t ptr_mid = alloc_x(shm_type, &ptr);
            CHECK_LOG_RETVAL(ptr != nullptr && ptr_mid != UINT64_MAX, nullptr);

            memset(ptr, 0, item_size);
        }

        return ptr;
    }

    bool struct_is_inited(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX && g_shm_mgr != nullptr, false);
        return (g_shm_mgr->addr_pool[shm_type].item_base_num <= 0) ? false : true;
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    uint64_t alloc_x(int shm_type, void** ptr)
    {
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX && g_shm_mgr != nullptr, UINT64_MAX);
        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_type];

        if (pool.item_base_num <= 0) return UINT64_MAX;

        int32_t ret = 0;
        int32_t& current_index = pool.cur_node_index;

        if (current_index == -1)
        {
            ret = _shm_try_to_map_node(pool, current_index + 1);
            if (ret != 0)
            {
                LOG_ERROR("shm[%s] dynamic pool is failed to map pool node, ret[%d]", type2name(shm_type), ret);
                return UINT64_MAX;
            }
        }

        CHECK_LOG_RETVAL(current_index >= 0 && current_index < POOL_NODE_COUNT&& is_file_handle_valid(pool.node_array[current_index]), UINT64_MAX);
        SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[current_index].addr;

        //定制: 到达上限
        if (pool.item_total_num >= 0 && pool.item_mapped_num - pool.item_free_num >= pool.item_total_num)
        {
            LOG_ERROR("shm[%s] dynamic pool is full total_num[%lld], used_num[%lld]", type2name(shm_type),
                pool.item_total_num, pool.item_mapped_num - pool.item_free_num);
            return UINT64_MAX;
        }

        //说明分配完了
        if (pool_node->head_item_offset == 0)
        {
            //保护型代码
            CHECK_LOG_RETVAL(pool_node->free_item_num == 0, UINT64_MAX);
            //向前滚动
            ret = _shm_try_to_map_node(pool, current_index + 1);
            if (ret != 0)
            {
                LOG_ERROR("shm[%s] dynamic pool is failed to map pool node, ret[%d]", type2name(shm_type), ret);
                return UINT64_MAX;
            }
        }

        pool_node = (SHM_STRUCT_HEAD*) pool.node_array[current_index].addr;
        CHECK_LOG_RETVAL(pool_node != nullptr, UINT64_MAX);

        SHM_STRUCT* item = (SHM_STRUCT*) OFFSET2PTR(pool_node, pool_node->head_item_offset);
        //让head指向下一个item
        pool_node->head_item_offset = item->next_item_offset;
        --pool_node->free_item_num;
        --pool.item_free_num;

        //分配序列号
        int32_t real_serial = (++pool_node->item_serial) & UNIT_SERIAL_MASK;

        if (real_serial <= 0)
        {
            pool_node->item_serial = 1;
            real_serial = 1;
        }

        //说明这个item正在使用
        item->next_item_offset = 0;
        item->item_serial = real_serial;

        SHM_STRUCT_MID mid;
        mid.item_serial = item->item_serial;
        mid.shm_type = shm_type;
        mid.node_index = current_index;
        mid.item_offset = PTR2OFFSET(pool_node, item);

        if (ptr != nullptr) *ptr = item->item_data;
        return *(uint64_t*) &mid;
    }

    int free_x(uint64_t mid)
    {
        // 无效的mid
        if (mid == 0ull || mid == UINT64_MAX)  return 0;

        SHM_STRUCT_MID shm_mid = *(SHM_STRUCT_MID*) &mid;
        CHECK_LOG_RETVAL(shm_mid.shm_type < SHM_TYPE_MAX, -1);
        //只占5位所以一定满足
        CHECK_LOG_RETVAL(shm_mid.item_offset != 0, -1);
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, -1);

        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_mid.shm_type];
        CHECK_LOG_RETVAL(pool.item_data_size > 0 && pool.item_real_data_size > 0 && pool.item_base_num > 0, -1);
        CHECK_LOG_RETVAL(is_file_handle_valid(pool.node_array[shm_mid.node_index]), -1);

        SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[shm_mid.node_index].addr;
        SHM_STRUCT* item = (SHM_STRUCT*) OFFSET2PTR(pool_node, shm_mid.item_offset);
        CHECK_LOG_RETVAL(shm_mid.item_serial == item->item_serial, -1);

        if (pool_node->head_item_offset == 0)
        {
            // head为0, 说明链表里面本身没有item
            pool_node->head_item_offset = shm_mid.item_offset;
            item->next_item_offset = 0;
        }
        else
        {
            //否则item放在head前面
            item->next_item_offset = pool_node->head_item_offset;
            pool_node->head_item_offset = shm_mid.item_offset;
        }

        item->item_serial = 0;
        ++pool_node->free_item_num;
        ++pool.item_free_num;

        if (pool_node->free_item_num >= pool_node->mapped_item_num)
            pool.dirty_flag = TRUE;
        if (pool.dirty_flag)
            _shm_try_to_unmap_node(pool, shm_mid.node_index);
        return 0;
    }

    bool is_valid_mid(uint64_t mid)
    {
        return mid2ptr(mid) != nullptr;
    }

    bool can_alloc(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, false);
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, false);

        if (g_shm_mgr->addr_pool[shm_type].item_base_num <= 0)
            return false;

        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_type];
        if (pool.item_total_num >= 0 && pool.item_mapped_num - pool.item_free_num >= pool.item_total_num)
        {
            return false;
        }
        return true;
    }

    bool can_alloc(int32_t shm_type, int64_t alloc_num)
    {
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, false);
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, false);

        if (g_shm_mgr->addr_pool[shm_type].item_base_num <= 0)
            return false;

        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_type];
        if (pool.item_total_num >= 0 && (pool.item_mapped_num - pool.item_free_num + alloc_num) > pool.item_total_num)
        {
            return false;
        }

        return true;
    }

    void* mid2ptr(uint64_t mid)
    {
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, nullptr);

        if (mid == UINT64_MAX)
            return nullptr;

        SHM_STRUCT_MID shm_mid = *(SHM_STRUCT_MID*) &mid;
        if (shm_mid.shm_type >= SHM_TYPE_MAX)
            return nullptr;

        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_mid.shm_type];
        SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[shm_mid.node_index].addr;

        if (pool_node == nullptr)
            return nullptr;

        // 判断长度是否越界
        uint64_t max_size = ALIGN8(SIZE_STRUCT_HEAD + (pool.item_real_data_size * pool_node->mapped_item_num));
        if (shm_mid.item_offset < SIZE_STRUCT_HEAD || shm_mid.item_offset >= max_size)
        {
            LOG_ERROR("shm type %llu, node idx %llu, offset %llu, out of range [%llu : %llu]"
                , (uint64_t) shm_mid.shm_type
                , (uint64_t) shm_mid.node_index
                , (uint64_t) shm_mid.item_offset
                , (uint64_t) SIZE_STRUCT_HEAD
                , max_size);
            return nullptr;
        }

        // 判断offset是否合法
        if (((shm_mid.item_offset - SIZE_STRUCT_HEAD) % pool.item_real_data_size) != 0)
        {
            LOG_ERROR("shm type %llu, node idx %llu, offset %llu invalid"
                , (uint64_t) shm_mid.shm_type
                , (uint64_t) shm_mid.node_index
                , (uint64_t) shm_mid.item_offset);
            return nullptr;
        }

        SHM_STRUCT* item = (SHM_STRUCT*) OFFSET2PTR(pool_node, shm_mid.item_offset);

        if (item->item_serial != shm_mid.item_serial)
            return nullptr;

        return item->item_data;
    }

    void* idx2ptr(int32_t shm_type, int64_t idx)
    {
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, nullptr);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, nullptr);

        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_type];

        for (int32_t i = 0; i < POOL_NODE_COUNT; ++i)
        {
            if (!is_file_handle_valid(pool.node_array[i]))
                continue;

            SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[i].addr;
            if (idx < pool_node->mapped_item_num)
            {
                SHM_STRUCT* item = (SHM_STRUCT*) ((int64_t) pool_node->lst_struct + (int64_t) pool.item_real_data_size * idx);
                return (item->item_serial != 0) ? (void*) item->item_data : nullptr;
            }
            idx -= pool_node->mapped_item_num;
        }

        return nullptr;
    }

    uint64_t idx2mid(int32_t shm_type, int64_t idx)
    {
        CHECK_LOG_RETVAL(g_shm_mgr != nullptr, UINT64_MAX);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, UINT64_MAX);

        SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[shm_type];
        for (int32_t i = 0; i < POOL_NODE_COUNT; ++i)
        {
            if (!is_file_handle_valid(pool.node_array[i]))
                continue;

            SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[i].addr;
            if (idx < pool_node->mapped_item_num)
            {
                SHM_STRUCT* item = (SHM_STRUCT*) ((int64_t) pool_node->lst_struct + (int64_t) pool.item_real_data_size * idx);
                if (item != nullptr && item->item_serial != 0)
                {
                    SHM_STRUCT_MID shm_mid;
                    shm_mid.item_serial = item->item_serial;
                    shm_mid.shm_type = shm_type;
                    shm_mid.node_index = i;
                    shm_mid.item_offset = PTR2OFFSET(pool_node, item);
                    return *(uint64_t*) &shm_mid;
                }
                return UINT64_MAX;
            }
            idx -= pool_node->mapped_item_num;
        }
        return UINT64_MAX;
    }

    int32_t mid2type(uint64_t mid)
    {
        SHM_STRUCT_MID* shm_mid = (SHM_STRUCT_MID*) &mid;
        return shm_mid->shm_type;
    }



    void set_last_timer(int32_t timer_type)
    {
        g_shm_mgr->last_info.last_timer = timer_type;
        g_shm_mgr->last_info.from_id = 0;
        g_shm_mgr->last_info.last_msg = 0;

        return;
    }

    void set_last_msg(int32_t from_id, uint16_t msg_id)
    {
        g_shm_mgr->last_info.from_id = from_id;
        g_shm_mgr->last_info.last_msg = msg_id;
        g_shm_mgr->last_info.last_timer = 0;

        return;
    }

    void delete_file_all(const char* name)
    {
        int64_t size = ALIGN8(sizeof(SHM_MGR));
        int64_t shm_size = 0;

        if (get_shm_mgr() == nullptr)
        {
            for (int i = 0; i < SHM_TYPE_MAX; ++i)
            {
                if (!is_exsit_type2name(i)) continue;
                for (int j = 0; j < POOL_NODE_COUNT; ++j)
                {
                   shm::shm_unlink_x(_shm_get_shm_name(i, j, name));
                }
            }
            shm::shm_unlink_x(name);
            g_shm_mgr = nullptr;
            file_handle_init(g_shm_handle);
        }
        else
        {
            for (int32_t i = 0; i < SHM_TYPE_MAX; ++i)
            {
                if (!is_exsit_type2name(i)) continue;
                SHM_STRUCT_POOL& pool = g_shm_mgr->addr_pool[i];
                for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
                {
                    if (is_file_handle_empty(pool.node_array[j]))  continue;
                    shm::shm_unlink_x(_shm_get_shm_name(pool.shm_type, j));
                }
            }
            shm::shm_unlink_x(get_shm_name());
            g_shm_mgr = nullptr;
            file_handle_init(g_shm_handle);
        }
    }
}
