#include "pch.h"
#include "shm/shm_struct_func.h"
#include "shm/shm_def.h"
#include "log/LogMgr.h"
#include "shm/shm_mgr.h"
#include "math/math_basic.h"
#include "math/math_func.h"
#include "shm/shm_struct_def.h"

namespace shm_mgr
{
    int32_t get_item_mem_size(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);
        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        return (pool.item_data_size > 0) ? pool.item_data_size : 0;
    }

    int32_t get_item_real_mem_size(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);
        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        return (pool.item_real_data_size > 0) ? pool.item_real_data_size : 0;
    }

    int64_t get_mapped_num(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);
        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        return pool.item_mapped_num;
    }

    int64_t get_total_num(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);
        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        if (pool.item_total_num < 0)  return pool.item_mapped_num;
        // 返回max
        return min_x(pool.item_mapped_num, pool.item_total_num);
    }

    int64_t get_max_num(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);

        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        // 没配max, 返回SHM_POOL_UNIT_MAX
        if (pool.item_total_num < 0) return SHM_POOL_ITEM_MAX;
        // 返回max
        return pool.item_total_num;
    }

    int32_t set_max_num(int32_t shm_type, int64_t max_num)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);

        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        pool.item_total_num = max_num;

        return 0;
    }

    int64_t get_max_free_num(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);

        if (get_shm_mgr()->addr_pool[shm_type].item_base_num <= 0)
            return 0;

        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        if (pool.item_total_num >= 0)
        {
            if ((pool.item_mapped_num - pool.item_free_num) >= pool.item_total_num)
                return 0;
            else
                return pool.item_total_num - (pool.item_mapped_num - pool.item_free_num);
        }
        else
        {
            CHECK_LOG_RETVAL((pool.item_mapped_num - pool.item_free_num) < SHM_POOL_ITEM_MAX, 0);
            return SHM_POOL_ITEM_MAX - (pool.item_mapped_num - pool.item_free_num);
        }
    }
    std::string _shm_get_shm_name(int32_t shm_type, int32_t index, const char* sh_name)
    {
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, "");
        CHECK_LOG_RETVAL(index >= 0 && index < POOL_NODE_COUNT, "");

        std::string strName;
        if (sh_name == nullptr)
            strName = get_shm_name();
        else
            strName = sh_name;

        strName += "-";
        if (is_exsit_type2name(shm_type))
        {
            strName += type2name(shm_type);
        }
        else
        {
            strName += type2name(SHM_TYPE_NULL);
            strName += std::to_string(shm_type);
        }
        strName = strName + "." + std::to_string(index);
        strName = str_to_lower(strName);
        return strName;
    }

    int64_t get_free_num(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);

        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        return pool.item_free_num;
    }

    int64_t get_used_num(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);
        CHECK_LOG_RETVAL(shm_type >= 0 && shm_type < SHM_TYPE_MAX, 0);

        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        return (pool.item_mapped_num - pool.item_free_num);
    }

    uint64_t get_struct_mem_size()
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);

        uint64_t size = 0;
        for (int32_t i = 0; i < SHM_TYPE_MAX; ++i)
        {
            SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[i];
            for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
            {
                if (!is_file_handle_valid(pool.node_array[j]))
                    continue;

                SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[j].addr;
                assert_continue(pool_node != nullptr);
                size += ALIGN8(SIZE_STRUCT_HEAD + (pool.item_real_data_size * pool_node->mapped_item_num));
            }
        }

        return size;
    }

    uint64_t get_struct_used_mem_size()
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);

        uint64_t size = 0;

        for (int32_t i = 0; i < SHM_TYPE_MAX; ++i)
        {
            SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[i];
            for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
            {
                if (!is_file_handle_valid(pool.node_array[j]))
                    continue;

                SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[j].addr;
                assert_continue(pool_node != nullptr);

                int64_t pool_node_used_num = pool_node->mapped_item_num - pool_node->free_item_num;
                size += ALIGN8(SIZE_STRUCT_HEAD + (pool.item_real_data_size * pool_node_used_num));
            }
        }

        return size;
    }

    uint64_t get_struct_mem_size(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);

        uint64_t size = 0;
        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];
        for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
        {
            if (!is_file_handle_valid(pool.node_array[j]))
                continue;

            SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[j].addr;
            assert_continue(pool_node != nullptr);

            size += ALIGN8(SIZE_STRUCT_HEAD + (pool.item_real_data_size * pool_node->mapped_item_num));
        }

        return size;
    }

    uint64_t get_struct_used_mem_size(int32_t shm_type)
    {
        CHECK_LOG_RETVAL(get_shm_mgr() != nullptr, 0);

        uint64_t size = 0;

        SHM_STRUCT_POOL& pool = get_shm_mgr()->addr_pool[shm_type];

        for (int32_t j = 0; j < POOL_NODE_COUNT; ++j)
        {
            if (!is_file_handle_valid(pool.node_array[j]))
                continue;

            SHM_STRUCT_HEAD* pool_node = (SHM_STRUCT_HEAD*) pool.node_array[j].addr;
            assert_continue(pool_node != nullptr);

            size += ALIGN8(SIZE_STRUCT_HEAD
                + (pool.item_real_data_size * (pool_node->mapped_item_num - pool_node->free_item_num)));
        }

        return size;
    }
}