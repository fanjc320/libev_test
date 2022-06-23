#pragma once
#include <stdint.h>
#include "math/math_func.h"
#include "container/fix_utility.h"
#include "container/fix_base.h"
#include "log/LogMgr.h"
#include "math/math_func.h"
#include <string>

namespace fix
{
    template<typename K, typename V, size_t N>
    struct fix_hashmap_node
    {
        typedef typename size_type_auto<N>::index_t hash_index_t;
        static const hash_index_t npos = static_cast<hash_index_t>(-1);

        size_t      key = 0;
        hash_index_t next = npos;
        hash_index_t prev = npos;
        fpair<K, V> value;
        fix_hashmap_node(K& _key, V& _val)
        {
            key = hash_bytes(_key);
            next = npos;
            prev = npos;
            value.first = _key;
            value.second = _val;
        }
    };

    template<typename K, typename V, size_t N>
    struct fix_hashmap
    {
        typedef K key_type;
        typedef V value_type;

        typedef fix_hashmap_node<K, V, N>   node;
        typedef fix_base<node, N>           impl_type;
        typedef fix_hashmap_node<K, V, N>::hash_index_t hash_index_t;

        static const hash_index_t npos = node::npos;
        static const uint32_t SIZE_BOUND = ALIGN8((N + 1) / 2);

        impl_type   _nodes;
        hash_index_t _hash[SIZE_BOUND];

        fix_hashmap()
        {
            _nodes.init();
            memset(_hash, -1, sizeof(_hash));
        }

        bool full()
        {
            return _nodes.full();
        }

        size_t size()
        {
            return _nodes.size();
        }

        node* find(K key)
        {
            size_t k = _hash_key(key);
            hash_index_t index = _buck_index(k);
            hash_index_t i = _hash[k];
            if (i == npos) return nullptr;

            node* p = nullptr;
            while (i != npos)
            {
                p = _nodes.at(i);
                if (p != nullptr && p->key == k) return p;
                if (p == nullptr) return nullptr;

                i = p->next;
            }

            return nullptr;
        }

        bool insert(K key, V val)
        {
            size_t k = _hash_key(key);
            hash_index_t index = _buck_index(k);
            hash_index_t i = _hash[index];
            hash_index_t j = i;
            node* p = nullptr;
            while (i != npos)
            {
                p = _nodes.at(i);
                if ((p != nullptr && p->key == k) || p == nullptr) return false;
                j = i;
                i = p->next;
            }

            fix::fpair<node*, size_t> pInsert = _nodes.emplace(key, val);
            if (pInsert.first != nullptr)
            {
                if (p != nullptr)
                {
                    p->next = (hash_index_t) pInsert.second;
                    pInsert.first->prev = j;
                }
                else
                {
                    _hash[index] = (hash_index_t) pInsert.second;
                }
            }
            return true;
        }

        void erase(K key)
        {
            size_t k = _hash_key(key);
            hash_index_t index = _buck_index(k);
            hash_index_t i = _hash[index];
            hash_index_t j = i;
            if (i == npos) return;

            node* p = nullptr;
            while (i != npos)
            {
                p = _nodes.at(i);
                if ((p != nullptr && p->key == k) || p == nullptr) break;
                j = i;
                i = p->next;
            }

            if (p == nullptr || p->key != key) return;
            if (p->prev == npos)
            {
                _hash[index] = (hash_index_t) p->next;
            }
            else
            {
                node* pPrev = _nodes.at(p->prev);
                CHECK_LOG_RETNONE(pPrev != nullptr);
                pPrev->next = (hash_index_t) p->next;
            }

            if (p->next != npos)
            {
                node* pNext = _nodes.at(p->next);
                CHECK_LOG_RETNONE(pNext != nullptr);
                pNext->prev = p->prev;
            }
            _nodes.erase(j);
        }

        void clear()
        {
            _nodes.clear();
            memset(_hash, -1, sizeof(_hash));
        }

    private:
        hash_index_t _buck_index(size_t key)
        {
            return static_cast<hash_index_t>(key % SIZE_BOUND);
        }

        size_t _hash_key(key_type key)
        {
            return hash_bytes(key);
        }
    };
};