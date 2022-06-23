#pragma once

#include <limits.h>

#include <string>
#include "dtype.h"
#include <stdint.h>
#include "math/math_basic.h"
#include "container/fix_bit.h"
#include <iostream>
#include "container/fix_utility.h"

namespace fix
{
    const int32_t MAX_SL_PART = 4;//每四个项升一级
    const int32_t MAX_SL_LEVEL = 12; //最大跳表级别

    template <size_t N, size_t Low = 1, size_t Upp = N>
    struct skip_list_level
    {
        static const size_t MID = (Low + Upp) / 2;
        static const size_t MID_LVL = MID > MAX_SL_LEVEL ? MAX_SL_LEVEL : MID;
        static const bool   DOWN = (square_count<MAX_SL_PART, MID_LVL>::RET > N);
        static const size_t VAL = skip_list_level<N, (DOWN ? Low : MID_LVL + 1), (DOWN ? MID_LVL : Upp)>::VAL;
    };

    template <size_t N, size_t Mid>
    struct skip_list_level<N, Mid, Mid>
    {
        static const size_t VAL = Mid;
    };

    template <class T, int N, class K>
    struct skip_list_node
    {
    public:
        K key;
        T elem;
        typedef size_type_auto<N>::index_t list_index_t;
        list_index_t right[skip_list_level<N>::VAL];
    };

    template <class T, int N, class K = int>
    struct fix_skip_list
    {
    private:
        typedef skip_list_node<T, N, K> skip_list_t;

    private:
        skip_list_t _values[N] = {};
        fix_bit_list<N> _bitmap = {};

        skip_list_t* get_node_by_idx(int idx);
        const skip_list_t* get_node_by_idx(int idx) const;
        typedef skip_list_node<T, N, K>::list_index_t list_index_t;
        list_index_t right[skip_list_level<N>::VAL];
        list_index_t free_list = 0;

    public:
        void    init();
        int     insert(K key, const T& value);

        T* get(const K& key);
        const T* get(const K& key) const;
        T* get_by_idx(int idx);
        T& operator[](const K& key);

        int     get_idx(const K& key);
        int     erase(const K& key);
        size_t  count();
        bool    is_no_pos(const T& elem) const;
        bool    full() const;
        bool    empty() const;

        std::string to_string();
    public:
        struct iterator
        {
            iterator(int pos, fix_skip_list<T, N, K>* cont);
            typename fix_skip_list<T, N, K>::iterator& operator++();
            typename fix_skip_list<T, N, K>::iterator operator++(int);
            bool operator==(const fix_skip_list<T, N, K>::iterator& it);
            bool operator!=(const fix_skip_list<T, N, K>::iterator& it);
            T& operator()();
            K& get_key();
            int pos() const;
            int _pos;
            fix_skip_list<T, N, K>* _cont;
        };

        typename fix_skip_list<T, N, K>::iterator begin();
        typename fix_skip_list<T, N, K>::iterator end();
        typename fix_skip_list<T, N, K>::iterator find(const K& key);

        typedef bool (*FUNC_CONT_FILTER)(const T&);

        struct filter_iterator : public iterator
        {
            filter_iterator(int pos, fix_skip_list<T, N, K>* cont, FUNC_CONT_FILTER filter);
            typename fix_skip_list<T, N, K>::filter_iterator& operator++();
            typename fix_skip_list<T, N, K>::filter_iterator operator++(int);
            bool operator==(const fix_skip_list<T, N, K>::filter_iterator& it);
            bool operator!=(const fix_skip_list<T, N, K>::filter_iterator& it);

        private:
            FUNC_CONT_FILTER _filter;
        };

        typename fix_skip_list<T, N, K>::filter_iterator filter_begin(FUNC_CONT_FILTER filter);
        typename fix_skip_list<T, N, K>::filter_iterator filter_end(FUNC_CONT_FILTER filter);

        struct const_iterator
        {
            const_iterator(const iterator& itr);
            const_iterator(int pos, const fix_skip_list<T, N, K>* cont);
            typename fix_skip_list<T, N, K>::const_iterator& operator++();
            typename fix_skip_list<T, N, K>::const_iterator operator++(int);
            bool operator==(const fix_skip_list<T, N, K>::const_iterator& it);
            bool operator!=(const fix_skip_list<T, N, K>::const_iterator& it);
            const T& operator()();
            const K& get_key();
            int pos() const;
            int _pos;
            const fix_skip_list<T, N, K>* _cont;
        };

        typename fix_skip_list<T, N, K>::const_iterator begin() const;
        typename fix_skip_list<T, N, K>::const_iterator end() const;
        typename fix_skip_list<T, N, K>::const_iterator find(const K& key) const;

        struct const_filter_iterator : public const_iterator
        {
            const_filter_iterator(int pos, const fix_skip_list<T, N, K>* cont,
                FUNC_CONT_FILTER filter);
            typename fix_skip_list<T, N, K>::const_filter_iterator& operator++();
            typename fix_skip_list<T, N, K>::const_filter_iterator operator++(int);
            bool operator==(const fix_skip_list<T, N, K>::const_filter_iterator& it);
            bool operator!=(const fix_skip_list<T, N, K>::const_filter_iterator& it);

        private:
            FUNC_CONT_FILTER _filter;
        };

        typename fix_skip_list<T, N, K>::const_filter_iterator
            filter_begin(FUNC_CONT_FILTER filter) const;
        typename fix_skip_list<T, N, K>::const_filter_iterator
            filter_end(FUNC_CONT_FILTER filter) const;

        static T* _no_pos_item;
        static K* _no_pos_key;
    };

    template <class T, int N, class K>
    T* fix_skip_list<T, N, K>::_no_pos_item = new T();
    template <class T, int N, class K>
    K* fix_skip_list<T, N, K>::_no_pos_key = new K();

    inline int skiplist_level_idx(int idx)
    {
        if ((idx + 1) % MAX_SL_PART)
        {
            return 0;
        }
        else
        {
            return 1 + skiplist_level_idx((idx + 1) / MAX_SL_PART - 1);
        }
    }

    template <class T, int N, class K>
    void fix_skip_list<T, N, K>::init()
    {
        _bitmap.clear();
        for (size_t i = 0; i < skip_list_level<N>::VAL; i++)
        {
            right[i] = (list_index_t) -1;
        }
        for (size_t i = 0; i < N; i++)
        {
            _values[i].right[0] = (list_index_t) (i + 1); // right[0] as free_list
        }
        free_list = 0;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::skip_list_t*
        fix_skip_list<T, N, K>::get_node_by_idx(int idx)
    {
        if (unlikely(idx < 0 || idx >= N))
        {
            return NULL;
        }
        return &_values[idx];
    }

    template <class T, int N, class K>
    const typename fix_skip_list<T, N, K>::skip_list_t*
        fix_skip_list<T, N, K>::get_node_by_idx(int idx) const
    {
        if (unlikely(idx < 0 || idx >= N))
        {
            return NULL;
        }
        return &_values[idx];
    }

    template <class T, int N, class K>
    int fix_skip_list<T, N, K>::insert(K key, const T& value)
    {
        skip_list_t* update[skip_list_level<N>::VAL] = { NULL };
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                update[i] = cur_node;
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            if (!right_node)
            {
                update[i] = cur_node;
                continue;
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    right_node->elem = value;
                    return 0;
                }
                cur_node = right_node;
                update[i] = right_node;
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }

        int idx = free_list;
        assert_retval(idx >= 0 && idx < N, -1);
        skip_list_t* skip_node = get_node_by_idx(idx);
        assert_retval(skip_node, -1);
        free_list = skip_node->right[0];
        skip_node->key = key;
        skip_node->elem = value;
        assert_retval(skiplist_level_idx(idx) < (int) skip_list_level<N>::VAL, -1);
        for (int i = 0; i <= skiplist_level_idx(idx); i++)
        {
            if (update[i])
            {
                skip_node->right[i] = update[i]->right[i];
                update[i]->right[i] = idx;
            }
            else
            {
                skip_node->right[i] = right[i];
                right[i] = idx;
            }
        }
        assert_retval(idx >= 0 && idx <= N, -1);
        _bitmap.set(static_cast<size_t>(idx));
        return 0;
    }

    template <class T, int N, class K>
    T* fix_skip_list<T, N, K>::get(const K& key)
    {
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    return &right_node->elem;
                }
                cur_node = right_node;
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        return NULL;
    }

    template <class T, int N, class K>
    const T* fix_skip_list<T, N, K>::get(const K& key) const
    {
        const skip_list_t* cur_node = NULL;
        const skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    return &right_node->elem;
                }
                cur_node = right_node;
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        return NULL;
    }

    template <class T, int N, class K>
    T* fix_skip_list<T, N, K>::get_by_idx(int idx)
    {
        if (_bitmap.check(idx))
        {
            skip_list_t* node = get_node_by_idx(idx);
            assert_retval(node, NULL);
            return &node->elem;
        }
        return NULL;
    }

    template <class T, int N, class K>
    T& fix_skip_list<T, N, K>::operator[](const K& key)
    {
        skip_list_t* update[skip_list_level<N>::VAL] = { NULL };
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                update[i] = cur_node;
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            if (!right_node)
            {
                update[i] = cur_node;
                continue;
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    return right_node->elem;
                }
                cur_node = right_node;
                update[i] = right_node;
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }

        int idx = free_list;
        assert_retval(idx >= 0 && idx < N, *_no_pos_item);
        skip_list_t* skip_node = get_node_by_idx(idx);
        assert_retval(skip_node, *_no_pos_item);
        free_list = skip_node->right[0];
        skip_node->key = key;
        skip_node->elem = T();

        assert_retval(skiplist_level_idx(idx) < (int) skip_list_level<N>::VAL, *_no_pos_item);
        for (int i = 0; i <= skiplist_level_idx(idx); i++)
        {
            if (update[i])
            {
                skip_node->right[i] = update[i]->right[i];
                update[i]->right[i] = (list_index_t) idx;
            }
            else
            {
                skip_node->right[i] = right[i];
                right[i] = (list_index_t) idx;
            }
        }
        assert_retval(idx >= 0 && idx <= N, *_no_pos_item);
        _bitmap.set(static_cast<size_t>(idx));
        return skip_node->elem;
    }

    template <class T, int N, class K>
    int fix_skip_list<T, N, K>::get_idx(const K& key)
    {
        int idx = -1;
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                idx = right[i];
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                idx = cur_node->right[i];
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    return idx;
                }
                cur_node = right_node;
                idx = right_node->right[i];
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        return -1;
    }

    template <class T, int N, class K>
    int fix_skip_list<T, N, K>::erase(const K& key)
    {
        int idx = -1;
        int tmp_idx = -1;
        skip_list_t* update[skip_list_level<N>::VAL] = { NULL };
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                tmp_idx = right[i];
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                update[i] = cur_node;
                tmp_idx = cur_node->right[i];
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            if (!right_node)
            {
                update[i] = cur_node;
                continue;
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    idx = tmp_idx;
                    break;
                }
                cur_node = right_node;
                update[i] = right_node;
                tmp_idx = right_node->right[i];
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        if (idx == -1)
        {
            return -1;
        }
        cur_node = get_node_by_idx(idx);
        assert_retval(cur_node, -1);
        assert_retval(skiplist_level_idx(idx) < (int) skip_list_level<N>::VAL, -1);
        for (int i = 0; i <= skiplist_level_idx(idx); i++)
        {
            if (update[i])
            {
                update[i]->right[i] = cur_node->right[i];
            }
            else
            {
                right[i] = cur_node->right[i];
            }
        }
        assert_retval(idx >= 0 && idx <= N, -1);
        _bitmap.unset(static_cast<size_t>(idx));
        cur_node->right[0] = free_list;
        free_list = (list_index_t) idx;
        return 0;
    }

    template <class T, int N, class K>
    size_t fix_skip_list<T, N, K>::count()
    {
        return (size_t) _bitmap.set_num();
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::is_no_pos(const T& elem) const
    {
        return &elem == _no_pos_item;
    };

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::full() const
    {
        return _bitmap.set_num() == N;
    };

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::empty() const
    {
        return _bitmap.empty();
    };

    template <class T, int N, class K>
    std::string fix_skip_list<T, N, K>::to_string()
    {
        std::string ss;
        iterator itr = begin();
        for (; itr != end(); ++itr)
        {
            if (itr != end())
            {
                skip_list_t* node = get_node_by_idx(itr._pos);
                if (node)
                {
                    ss = ss + std::to_string(itr._pos) + "," + std::to_string(node->key) + "; ";
                }
            }
        }
        return ss;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::iterator
        fix_skip_list<T, N, K>::begin()
    {
        return iterator(right[0], this);
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::iterator
        fix_skip_list<T, N, K>::end()
    {
        return iterator(-1, this);
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::iterator
        fix_skip_list<T, N, K>::find(const K& key)
    {
        return iterator(find(key), this);
    }

    template <class T, int N, class K>
    fix_skip_list<T, N, K>::iterator::iterator(
        int cont_pos, fix_skip_list<T, N, K>* cont)
        : _pos(cont_pos), _cont(cont)
    {
        assert_retnone(_cont != NULL);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::iterator&
        fix_skip_list<T, N, K>::iterator::operator++()
    {
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return *this;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::iterator
        fix_skip_list<T, N, K>::iterator::operator++(int)
    {
        fix_skip_list<T, N, K>::iterator tmp_itr = *this;
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (!_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return tmp_itr;
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::iterator::operator==(
        const fix_skip_list<T, N, K>::iterator& it)
    {
        return _cont == it._cont && _pos == it._pos;
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::iterator::operator!=(
        const fix_skip_list<T, N, K>::iterator& it)
    {
        return _cont != it._cont || _pos != it._pos;
    }

    template <class T, int N, class K>
    T& fix_skip_list<T, N, K>::iterator::operator()()
    {
        fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->elem;
        }
        else
        {
            return *fix_skip_list<T, N, K>::_no_pos_item;
        }
    }

    template <class T, int N, class K>
    K& fix_skip_list<T, N, K>::iterator::get_key()
    {
        fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->key;
        }
        else
        {
            return *fix_skip_list<T, N, K>::_no_pos_key;
        }
    }

    template <class T, int N, class K>
    int fix_skip_list<T, N, K>::iterator::pos() const
    {
        return _pos;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::filter_iterator
        fix_skip_list<T, N, K>::filter_begin(FUNC_CONT_FILTER filter)
    {
        typename fix_skip_list<T, N, K>::iterator base_itr = begin();
        while (base_itr != end())
        {
            const T& elem = base_itr();
            if (filter(elem))
            {
                return filter_iterator(base_itr.pos(), this, filter);
            }
            base_itr++;
        }
        return filter_iterator(base_itr.pos(), this, filter);
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::filter_iterator
        fix_skip_list<T, N, K>::filter_end(FUNC_CONT_FILTER filter)
    {
        return filter_iterator(-1, this, filter);
    }

    template <class T, int N, class K>
    fix_skip_list<T, N, K>::filter_iterator::filter_iterator(
        int cont_pos, fix_skip_list<T, N, K>* cont, FUNC_CONT_FILTER filter)
        : fix_skip_list<T, N, K>::iterator(cont_pos, cont), _filter(filter)
    { }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::filter_iterator&
        fix_skip_list<T, N, K>::filter_iterator::operator++()
    {
        fix_skip_list<T, N, K>* cont = this->_cont;
        int& pos = this->_pos;
        assert_retval(cont != NULL, *this);
        assert_noeffect(cont->_bitmap.check(pos) == true);
        iterator base_itr = *this;
        base_itr++;
        while (base_itr != cont->end())
        {
            const T& elem = base_itr();
            if (_filter(elem))
            {
                break;
            }
            base_itr++;
        };
        pos = base_itr.pos();
        return *this;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::filter_iterator
        fix_skip_list<T, N, K>::filter_iterator::operator++(int)
    {
        fix_skip_list<T, N, K>* cont = this->_cont;
        int& pos = this->_pos;
        assert_retval(cont != NULL, *this);
        assert_noeffect(cont->_bitmap.check(pos) == true);
        fix_skip_list<T, N, K>::filter_iterator tmp_itr = *this;
        iterator base_itr = *this;
        base_itr++;
        while (base_itr != cont->end())
        {
            const T& elem = base_itr();
            if (_filter(elem))
            {
                break;
            }
            base_itr++;
        };
        pos = base_itr.pos();
        return tmp_itr;
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::filter_iterator::operator==(
        const fix_skip_list<T, N, K>::filter_iterator& it)
    {
        assert_noeffect(_filter == it._filter);
        return fix_skip_list<T, N, K>::iterator(*this)
            == fix_skip_list<T, N, K>::iterator(it);
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::filter_iterator::operator!=(
        const fix_skip_list<T, N, K>::filter_iterator& it)
    {
        assert_noeffect(_filter == it._filter);
        return fix_skip_list<T, N, K>::iterator(*this)
            != fix_skip_list<T, N, K>::iterator(it);
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_iterator
        fix_skip_list<T, N, K>::begin() const
    {
        return const_iterator(right[0], this);
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_iterator
        fix_skip_list<T, N, K>::end() const
    {
        return const_iterator(-1, this);
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_iterator
        fix_skip_list<T, N, K>::find(const K& key) const
    {
        return const_iterator(find(key), this);
    }

    template <class T, int N, class K>
    fix_skip_list<T, N, K>::const_iterator::const_iterator(
        int cont_pos, const fix_skip_list<T, N, K>* cont)
        : _pos(cont_pos), _cont(cont)
    {
        assert_retnone(_cont != NULL);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_iterator&
        fix_skip_list<T, N, K>::const_iterator::operator++()
    {
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            const fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return *this;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_iterator
        fix_skip_list<T, N, K>::const_iterator::operator++(int)
    {
        fix_skip_list<T, N, K>::const_iterator tmp_itr = *this;
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            const fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return tmp_itr;
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::const_iterator::operator==(
        const fix_skip_list<T, N, K>::const_iterator& it)
    {
        return _cont == it._cont && _pos == it._pos;
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::const_iterator::operator!=(
        const fix_skip_list<T, N, K>::const_iterator& it)
    {
        return _cont != it._cont || _pos != it._pos;
    }

    template <class T, int N, class K>
    const T& fix_skip_list<T, N, K>::const_iterator::operator()()
    {
        const fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->elem;
        }
        else
        {
            return *fix_skip_list<T, N, K>::_no_pos_item;
        }
    }

    template <class T, int N, class K>
    const K& fix_skip_list<T, N, K>::const_iterator::get_key()
    {
        const fix_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->key;
        }
        else
        {
            return *fix_skip_list<T, N, K>::_no_pos_key;
        }
    }

    template <class T, int N, class K>
    int fix_skip_list<T, N, K>::const_iterator::pos() const
    {
        return _pos;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_filter_iterator
        fix_skip_list<T, N, K>::filter_begin(FUNC_CONT_FILTER filter) const
    {
        typename fix_skip_list<T, N, K>::const_iterator base_itr = begin();
        while (base_itr != end())
        {
            const T& elem = base_itr();
            if (filter(elem))
            {
                return const_filter_iterator(base_itr.pos(), this, filter);
            }
            base_itr++;
        }
        return const_filter_iterator(base_itr.pos(), this, filter);
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_filter_iterator
        fix_skip_list<T, N, K>::filter_end(FUNC_CONT_FILTER filter) const
    {
        return const_filter_iterator(-1, this, filter);
    }

    template <class T, int N, class K>
    fix_skip_list<T, N, K>::const_filter_iterator::const_filter_iterator(
        int cont_pos, const fix_skip_list<T, N, K>* cont, FUNC_CONT_FILTER filter)
        : fix_skip_list<T, N, K>::const_iterator(cont_pos, cont), _filter(filter)
    { }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_filter_iterator&
        fix_skip_list<T, N, K>::const_filter_iterator::operator++()
    {
        const fix_skip_list<T, N, K>* cont = this->_cont;
        int& pos = this->_pos;
        assert_retval(cont != NULL, *this);
        assert_noeffect(cont->_bitmap.check(pos) == true);
        const_iterator base_itr = *this;
        base_itr++;
        while (base_itr != cont->end())
        {
            const T& elem = base_itr();
            if (_filter(elem))
            {
                pos = base_itr.pos();
                return *this;
            }
            base_itr++;
        };
        pos = base_itr.pos();
        return *this;
    }

    template <class T, int N, class K>
    typename fix_skip_list<T, N, K>::const_filter_iterator
        fix_skip_list<T, N, K>::const_filter_iterator::operator++(int)
    {
        const fix_skip_list<T, N, K>* cont = this->_cont;
        int& pos = this->_pos;
        assert_retval(cont != NULL, *this);
        assert_noeffect(cont->_bitmap.check(pos) == true);
        fix_skip_list<T, N, K>::const_filter_iterator tmp_itr = *this;
        const_iterator base_itr = *this;
        base_itr++;
        while (base_itr != fix_skip_list<T, N, K>::const_iterator::_cont->end())
        {
            const T& elem = base_itr();
            if (_filter(elem))
            {
                pos = base_itr.pos();
                break;
            }
            base_itr++;
        };
        pos = base_itr.pos();
        return tmp_itr;
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::const_filter_iterator::operator==(
        const fix_skip_list<T, N, K>::const_filter_iterator& it)
    {
        assert_noeffect(_filter == it._filter);
        return fix_skip_list<T, N, K>::const_iterator(*this)
            == fix_skip_list<T, N, K>::const_iterator(it);
    }

    template <class T, int N, class K>
    bool fix_skip_list<T, N, K>::const_filter_iterator::operator!=(
        const fix_skip_list<T, N, K>::const_filter_iterator& it)
    {
        assert_noeffect(_filter == it._filter);
        return fix_skip_list<T, N, K>::const_iterator(*this)
            != fix_skip_list<T, N, K>::const_iterator(it);
    }

    template <class T, int N, class K>
    struct multi_skip_list_node
    {
    public:
        K key;
        T elem;
        typedef size_type_auto<N>::index_t list_index_t;

        list_index_t left; //多重跳表需要向前索引
        list_index_t right[skip_list_level<N>::VAL];
    };

    template <class T, int N, class K = int>
    struct fix_multi_skip_list
    {
    private:
        typedef multi_skip_list_node<T, N, K> skip_list_t;

    private:
        skip_list_t _values[N] = {};

        skip_list_t* get_node_by_idx(int idx);
        const skip_list_t* get_node_by_idx(int idx) const;

        fix_bit_list<N> _bitmap;
        typedef multi_skip_list_node<T, N, K>::list_index_t list_index_t;
        list_index_t right[skip_list_level<N>::VAL];
        list_index_t free_list = 0;

    public:
        struct iterator
        {
            iterator(int pos, fix_multi_skip_list<T, N, K>* cont);
            typename fix_multi_skip_list<T, N, K>::iterator& operator++();
            typename fix_multi_skip_list<T, N, K>::iterator operator++(int);
            typename fix_multi_skip_list<T, N, K>::iterator& operator--();
            typename fix_multi_skip_list<T, N, K>::iterator operator--(int);
            bool operator==(const fix_multi_skip_list<T, N, K>::iterator& it);
            bool operator!=(const fix_multi_skip_list<T, N, K>::iterator& it);
            T& operator()();
            K& get_key();
            int pos() const;
            int _pos;
            fix_multi_skip_list<T, N, K>* _cont;
        };

        typename fix_multi_skip_list<T, N, K>::iterator begin();
        typename fix_multi_skip_list<T, N, K>::iterator end();
        typename fix_multi_skip_list<T, N, K>::iterator rbegin();
        typename fix_multi_skip_list<T, N, K>::iterator rend();
        typename fix_multi_skip_list<T, N, K>::iterator find(const K& key);

        struct const_iterator
        {
            const_iterator(const iterator& itr);
            const_iterator(int pos, const fix_multi_skip_list<T, N, K>* cont);
            typename fix_multi_skip_list<T, N, K>::const_iterator& operator++();
            typename fix_multi_skip_list<T, N, K>::const_iterator operator++(int);
            bool operator==(const fix_multi_skip_list<T, N, K>::const_iterator& it);
            bool operator!=(const fix_multi_skip_list<T, N, K>::const_iterator& it);
            const T& operator()();
            const K& get_key();
            int pos() const;
            int _pos;
            const fix_multi_skip_list<T, N, K>* _cont;
        };

        typename fix_multi_skip_list<T, N, K>::const_iterator begin() const;
        typename fix_multi_skip_list<T, N, K>::const_iterator end() const;
        typename fix_multi_skip_list<T, N, K>::const_iterator rbegin() const;
        typename fix_multi_skip_list<T, N, K>::const_iterator rend() const;
        typename fix_multi_skip_list<T, N, K>::const_iterator find(const K& key) const;

    public:
        fix_multi_skip_list()
        {
            init();
        }
        ~fix_multi_skip_list()
        {
            _bitmap.clear();
            for (size_t i = 0; i < skip_list_level<N>::VAL; i++)
            {
                right[i] = -1;
            }
            for (size_t i = 0; i < N; i++)
            {
                _values[i].right[0] = (list_index_t) (i + 1); // right[0] as free_list
            }
            free_list = 0;
        }
        void    init();
        int     insert(K key, const T& value);
        T* get(const K& key);

        int     get_first_idx(const K& key);
        int     get_last_idx();
        int     erase(iterator& itr);
        size_t  count();
        bool    empty();
        bool    full();
        bool    is_no_pos(const T& elem) const;

        std::string to_string();
        static T* _no_pos_item;
        static K* _no_pos_key;
    };

    template <class T, int N, class K /*= int*/>
    bool fix::fix_multi_skip_list<T, N, K>::is_no_pos(const T& elem) const
    {
        return &elem == _no_pos_item;
    }

    template <class T, int N, class K>
    T* fix_multi_skip_list<T, N, K>::_no_pos_item = new T();
    template <class T, int N, class K>
    K* fix_multi_skip_list<T, N, K>::_no_pos_key = new K();

    template <class T, int N, class K>
    void fix_multi_skip_list<T, N, K>::init()
    {
        _bitmap.clear();
        for (size_t i = 0; i < skip_list_level<N>::VAL; i++)
        {
            right[i] = -1;
        }
        for (size_t i = 0; i < N; i++)
        {
            _values[i].right[0] = (list_index_t) (i + 1); // right[0] as free_list
        }
        free_list = 0;
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::skip_list_t*
        fix_multi_skip_list<T, N, K>::get_node_by_idx(int idx)
    {
        if (unlikely(idx < 0 || idx >= N))
        {
            return NULL;
        }
        return &_values[idx];
    }

    template <class T, int N, class K>
    const typename fix_multi_skip_list<T, N, K>::skip_list_t*
        fix_multi_skip_list<T, N, K>::get_node_by_idx(int idx) const
    {
        if (unlikely(idx < 0 || idx >= N))
        {
            return NULL;
        }
        return &_values[idx];
    }

    template <class T, int N, class K>
    int fix_multi_skip_list<T, N, K>::insert(K key, const T& value)
    {
        skip_list_t* update[skip_list_level<N>::VAL] = { NULL };
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        int left_idx = -1;
        int right_node_idx = -1;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                right_node_idx = right[i];
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                update[i] = cur_node;
                right_node_idx = cur_node->right[i];
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            if (!right_node)
            {
                update[i] = cur_node;
                continue;
            }
            while (right_node && right_node->key <= key)
            {
                cur_node = right_node;
                update[i] = right_node;
                left_idx = right_node_idx;
                right_node_idx = right_node->right[i];
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }

        int idx = free_list;
        assert_retval(idx >= 0 && idx < N, -1);
        skip_list_t* skip_node = get_node_by_idx(idx);
        assert_retval(skip_node, -1);
        free_list = skip_node->right[0];
        skip_node->key = key;
        skip_node->elem = value;
        assert_retval(skiplist_level_idx(idx) < (int) skip_list_level<N>::VAL, -1);
        for (int i = 0; i <= skiplist_level_idx(idx); i++)
        {
            if (update[i])
            {
                skip_node->right[i] = update[i]->right[i];
                update[i]->right[i] = (list_index_t) idx;
            }
            else
            {
                skip_node->right[i] = right[i];
                right[i] = (list_index_t) idx;
            }
        }
        skip_node->left = (list_index_t) left_idx;
        right_node = get_node_by_idx(skip_node->right[0]);
        if (right_node)
        {
            right_node->left = (list_index_t) idx;
        }
        assert_retval(idx >= 0 && idx <= N, -1);
        _bitmap.set(static_cast<size_t>(idx));
        return 0;
    }

    template <class T, int N, class K>
    T* fix_multi_skip_list<T, N, K>::get(const K& key)
    {
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    return &right_node->elem;
                }
                cur_node = right_node;
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        return NULL;
    }

    template <class T, int N, class K>
    int fix_multi_skip_list<T, N, K>::get_first_idx(const K& key)
    {
        int idx = -1;
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                idx = right[i];
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                idx = cur_node->right[i];
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    skip_list_t* left_node = right_node;
                    int test_idx = idx;
                    while (left_node && left_node->key == key)
                    {
                        idx = test_idx;
                        test_idx = left_node->left;
                        left_node = get_node_by_idx(left_node->left);
                    }
                    return idx;
                }
                cur_node = right_node;
                idx = right_node->right[i];
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        return -1;
    }

    template <class T, int N, class K>
    int fix_multi_skip_list<T, N, K>::get_last_idx()
    {
        int idx = -1;
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skip_list_level<N>::VAL - 1; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                right_node = get_node_by_idx(right[i]);
                if (right_node)
                {
                    idx = right[i];
                }
            }
            else
            {
                right_node = get_node_by_idx(cur_node->right[i]);
                if (right_node)
                {
                    idx = cur_node->right[i];
                }
            }
            while (right_node)
            {
                cur_node = right_node;
                idx = right_node->right[i] != -1 ? right_node->right[i] : idx;
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        return idx;
    }

    template <class T, int N, class K>
    int fix_multi_skip_list<T, N, K>::erase(fix_multi_skip_list::iterator& itr)
    {
        if (itr == end() || !_bitmap.check(itr.pos()))
        {
            return 0;
        }

        int idx = itr.pos();
        int tmp_idx = -1;
        int skiplist_level = skiplist_level_idx(idx);
        assert_retval(skiplist_level < (int) skip_list_level<N>::VAL&& skiplist_level >= 0, -1);
        K key = itr.get_key();
        skip_list_t* update[skip_list_level<N>::VAL] = { NULL };
        skip_list_t* cur_node = NULL;
        skip_list_t* right_node = NULL;
        for (int i = skiplist_level; i >= 0; i--)
        {
            if (cur_node == NULL)
            {
                tmp_idx = right[i];
                right_node = get_node_by_idx(right[i]);
            }
            else
            {
                update[i] = cur_node;
                tmp_idx = cur_node->right[i];
                right_node = get_node_by_idx(cur_node->right[i]);
            }
            if (!right_node)
            {
                update[i] = cur_node;
                continue;
            }
            while (right_node && right_node->key <= key)
            {
                if (right_node->key == key)
                {
                    break;
                }
                cur_node = right_node;
                update[i] = right_node;
                tmp_idx = right_node->right[i];
                right_node = get_node_by_idx(right_node->right[i]);
            }
        }
        int next_key_idx = update[0] ? update[0]->right[0] : right[0];
        while (true)
        {
            cur_node = get_node_by_idx(next_key_idx);
            if (cur_node == NULL)
            {
                return 0;
            }
            if (next_key_idx != idx)
            {
                for (int i = 0; i <= skiplist_level_idx(next_key_idx); i++)
                {
                    update[i] = cur_node;
                }
                next_key_idx = cur_node->right[0];
            }
            else
            {
                break;
            }
        }
        cur_node = get_node_by_idx(idx);
        assert_retval(cur_node, -1);
        right_node = get_node_by_idx(cur_node->right[0]);
        itr = fix_multi_skip_list::iterator(cur_node->right[0], this);
        if (right_node)
        {
            right_node->left = cur_node->left;
        }
        for (int i = 0; i <= skiplist_level_idx(idx); i++)
        {
            if (update[i])
            {
                update[i]->right[i] = cur_node->right[i];
            }
            else
            {
                right[i] = cur_node->right[i];
            }
        }
        assert_retval(idx >= 0 && idx <= N, -1);
        cur_node->right[0] = free_list;
        free_list = (list_index_t) idx;
        _bitmap.unset(static_cast<size_t>(idx));
        return 0;
    }

    template <class T, int N, class K>
    size_t fix_multi_skip_list<T, N, K>::count()
    {
        return _bitmap.set_num();
    }

    template <class T, int N, class K>
    bool fix_multi_skip_list<T, N, K>::empty()
    {
        return _bitmap.empty();
    }

    template <class T, int N, class K>
    bool fix_multi_skip_list<T, N, K>::full()
    {
        return _bitmap.set_num() == N;
    }

    template <class T, int N, class K>
    std::string fix_multi_skip_list<T, N, K>::to_string()
    {
        std::string ss;
        iterator itr = begin();
        for (; itr != end(); itr++)
        {
            skip_list_t* node = get_node_by_idx(itr._pos);
            if (node)
            {
                ss = ss + std::to_string(node->key) + ":" + std::to_string(node->elem) +
                    "," + std::to_string(itr._pos) + "," + (int) node->left + "; ";
            }
        }
        return ss;
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator
        fix_multi_skip_list<T, N, K>::begin()
    {
        return iterator(right[0], this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator
        fix_multi_skip_list<T, N, K>::end()
    {
        return iterator(-1, this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator
        fix_multi_skip_list<T, N, K>::rbegin()
    {
        return iterator(get_last_idx(), this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator
        fix_multi_skip_list<T, N, K>::rend()
    {
        return iterator(-1, this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator
        fix_multi_skip_list<T, N, K>::find(const K& key)
    {
        return iterator(get_first_idx(key), this);
    }

    template <class T, int N, class K>
    fix_multi_skip_list<T, N, K>::iterator::iterator(
        int cont_pos, fix_multi_skip_list<T, N, K>* cont)
        : _pos(cont_pos), _cont(cont)
    {
        assert_retnone(_cont != NULL);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator&
        fix_multi_skip_list<T, N, K>::iterator::operator++()
    {
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return *this;
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator
        fix_multi_skip_list<T, N, K>::iterator::operator++(int)
    {
        fix_multi_skip_list<T, N, K>::iterator tmp_itr = *this;
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return tmp_itr;
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator&
        fix_multi_skip_list<T, N, K>::iterator::operator--()
    {
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->get_last_idx();
        }
        else
        {
            fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->left;
            }
        }
        return *this;
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::iterator
        fix_multi_skip_list<T, N, K>::iterator::operator--(int)
    {
        fix_multi_skip_list<T, N, K>::iterator tmp_itr = *this;
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->get_last_idx();
        }
        else
        {
            fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->left;
            }
        }
        return tmp_itr;
    }

    template <class T, int N, class K>
    bool fix_multi_skip_list<T, N, K>::iterator::operator==(
        const fix_multi_skip_list<T, N, K>::iterator& it)
    {
        return _cont == it._cont && _pos == it._pos;
    }

    template <class T, int N, class K>
    bool fix_multi_skip_list<T, N, K>::iterator::operator!=(
        const fix_multi_skip_list<T, N, K>::iterator& it)
    {
        return _cont != it._cont || _pos != it._pos;
    }

    template <class T, int N, class K>
    T& fix_multi_skip_list<T, N, K>::iterator::operator()()
    {
        fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->elem;
        }
        else
        {
            return *fix_multi_skip_list<T, N, K>::_no_pos_item;
        }
    }

    template <class T, int N, class K>
    K& fix_multi_skip_list<T, N, K>::iterator::get_key()
    {
        fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->key;
        }
        else
        {
            return *fix_multi_skip_list<T, N, K>::_no_pos_key;
        }
    }

    template <class T, int N, class K>
    int fix_multi_skip_list<T, N, K>::iterator::pos() const
    {
        return _pos;
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::const_iterator
        fix_multi_skip_list<T, N, K>::begin() const
    {
        return const_iterator(right[0], this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::const_iterator
        fix_multi_skip_list<T, N, K>::end() const
    {
        return const_iterator(-1, this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::const_iterator
        fix_multi_skip_list<T, N, K>::rbegin() const
    {
        return const_iterator(get_last_idx(), this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::const_iterator
        fix_multi_skip_list<T, N, K>::rend() const
    {
        return const_iterator(-1, this);
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::const_iterator
        fix_multi_skip_list<T, N, K>::find(const K& key) const
    {
        return const_iterator(get_first_idx(key), this);
    }

    template <class T, int N, class K>
    fix_multi_skip_list<T, N, K>::const_iterator::const_iterator(
        const typename fix_multi_skip_list<T, N, K>::iterator& itr)
    {
        _cont = itr._cont;
        _pos = itr._pos;
    }

    template <class T, int N, class K>
    fix_multi_skip_list<T, N, K>::const_iterator::const_iterator(
        int cont_pos, const fix_multi_skip_list<T, N, K>* cont)
        : _pos(cont_pos), _cont(cont)
    {
        assert_retnone(_cont != NULL);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::const_iterator&
        fix_multi_skip_list<T, N, K>::const_iterator::operator++()
    {
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return *this;
    }

    template <class T, int N, class K>
    typename fix_multi_skip_list<T, N, K>::const_iterator
        fix_multi_skip_list<T, N, K>::const_iterator::operator++(int)
    {
        fix_multi_skip_list<T, N, K>::const_iterator tmp_itr = *this;
        assert_retval(_cont != NULL, *this);
        assert_noeffect(_cont->_bitmap.check(_pos) == true);
        if (_pos != -1 && !_cont->_bitmap.check(_pos))
        {
            _pos = -1;
        }
        if (_pos == -1)
        {
            _pos = _cont->right[0];
        }
        else
        {
            const fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
            if (node)
            {
                _pos = node->right[0];
            }
        }
        return tmp_itr;
    }

    template <class T, int N, class K>
    bool fix_multi_skip_list<T, N, K>::const_iterator::operator==(
        const fix_multi_skip_list<T, N, K>::const_iterator& it)
    {
        return _cont == it._cont && _pos == it._pos;
    }

    template <class T, int N, class K>
    bool fix_multi_skip_list<T, N, K>::const_iterator::operator!=(
        const fix_multi_skip_list<T, N, K>::const_iterator& it)
    {
        return _cont != it._cont || _pos != it._pos;
    }

    template <class T, int N, class K>
    const T& fix_multi_skip_list<T, N, K>::const_iterator::operator()()
    {
        const fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->elem;
        }
        else
        {
            return *fix_multi_skip_list<T, N, K>::_no_pos_item;
        }
    }

    template <class T, int N, class K>
    const K& fix_multi_skip_list<T, N, K>::const_iterator::get_key()
    {
        fix_multi_skip_list<T, N, K>::skip_list_t* node = _cont->get_node_by_idx(_pos);
        if (node)
        {
            return node->key;
        }
        else
        {
            return *fix_multi_skip_list<T, N, K>::_no_pos_key;
        }
    }

    template <class T, int N, class K>
    int fix_multi_skip_list<T, N, K>::const_iterator::pos() const
    {
        return _pos;
    }
};