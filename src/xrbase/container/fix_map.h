#pragma once

#include "container/fix_utility.h"
#include "container/fix_rbtree.h"

namespace fix
{
    template<typename K, typename V, size_t N>
    struct fix_map
    {
        typedef fix::fpair<const K, V>                                 value_type;
        typedef fix_rbtree<K, value_type, select1st<value_type>, N>   impl_type;
        typedef typename impl_type::iterator                          iterator;
        typedef typename impl_type::const_iterator                    const_iterator;
        typedef std::reverse_iterator<iterator>                       reverse_iterator;
        typedef std::reverse_iterator<const_iterator>                 const_reverse_iterator;

        impl_type tree;
        fix_map()
        {
            init();
        }
        ~fix_map()
        {
            clear();
        }
        bool init() { return tree.init(); }

        void uninit() { clear(); }

        bool empty() const { return tree.empty(); }

        bool full()  const { return tree.full(); }

        size_t size() const { return tree.size(); }

        size_t capacity() const { return tree.capacity(); }

        void clear() { tree.clear(); }

        V& operator[](const K& k)
        {
            iterator it = find(k);
            if (it == end())
            {
                int ret = insert(k, V());
                CHECK_LOG_RETVAL(ret == 0, *((V*) nullptr)); // I am sorry :(

                it = find(k);
                CHECK_LOG_RETVAL(it != end(), *((V*) nullptr)); // I am sorry too :(
            }

            return it->second;
        }

        int insert(const K& k, const V& v) { return tree.insert(value_type(k, v)); }

        void erase(iterator it) { tree.__erase(tree.__index(it.n)); }

        void erase(const_iterator it) { tree.__erase(tree.__index(it.n)); }

        void erase(const K& k) { tree.erase(k); }

        iterator find(const K& k) { return tree.find(k); }

        const_iterator find(const K& k) const { return tree.find(k); }

        iterator begin() { return tree.begin(); }
        iterator end() { return tree.end(); }
        const_iterator begin() const { return tree.begin(); }
        const_iterator end() const { return tree.end(); }

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    };
}