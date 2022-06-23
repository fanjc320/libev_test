#pragma once

namespace fix
{
    template<typename T, bool C>
    struct queue_iterator
    {
        typedef T                                                     queue_type;
        typedef typename T::value_type                                value_type;
        typedef queue_iterator<T, C>                                  self;
        typedef typename if_<C, const queue_type*, queue_type*>::type queue_pointer;
        typedef typename if_<C, const value_type*, value_type*>::type pointer;
        typedef typename if_<C, const value_type&, value_type&>::type reference;

        queue_pointer que;
        size_t cur_index;

        queue_iterator() : que(nullptr), cur_index(0) { }
        queue_iterator(queue_pointer que, size_t cur) : que(que), cur_index(cur) { }

        template<bool B>
        queue_iterator(queue_iterator<T, B>& s) : que(s.que), cur_index(s.cur_index) { }

        self& operator=(const self& s)
        {
            if (this == &s)
                return *this;

            que = s.que;
            cur_index = s.cur_index;

            return *this;
        }

        reference operator*() const { return *(que->at(cur_index)); }
        pointer operator->() const { return que->at(cur_index); }

        self& operator++() { cur_index++; return *this; }
        self operator++(int) { self tmp(*this); cur_index++; return tmp; }

        bool operator==(const self& x) const { return cur_index == x.cur_index && que == x.que; }
        bool operator!=(const self& x) const { return !(*this == x); }

        bool operator==(const queue_iterator<T, !C>& x) const { return cur_index == x.cur_index && que == x.que; }
        bool operator!=(const queue_iterator<T, !C>& x) const { return !(*this == x); }
    };

    template<typename T, size_t N>
    struct fix_queue
    {
        typedef T value_type;
        typedef fix_queue<T, N>                     self;
        typedef fix::queue_iterator<self, false>    iterator;
        typedef fix::queue_iterator<self, true>     const_iterator;
        static const size_t npos = static_cast<size_t>(-1);

        size_t used_count = 0;
        size_t bottom = 0;
        T memory[N] = {};

        fix_queue(){ }
        ~fix_queue() { clear(); }

        fix_queue(const fix_queue& queue) : used_count(0), bottom(0)
        {
            const_iterator it = queue.begin(), end = queue.end();
            for (; it != end; ++it)
                emplace(*it);
        }

        fix_queue& operator=(const fix_queue& queue)
        {
            if (this == &queue)
                return *this;

            clear();

            const_iterator it = queue.begin(), end = queue.end();
            for (; it != end; ++it)
                emplace(*it);

            return *this;
        }

        void init()
        {
            used_count = 0;
            bottom = 0;
        }

        void clear()
        {
            for (size_t i = 0; i < used_count; ++i)
                memory[i].~T();
            used_count = 0;
            bottom = 0;
        }

        size_t __index(size_t index) const { return (bottom + index) % (N); }
        size_t __next(size_t index) const { return __index(index + 1); }

        size_t size()     const { return used_count; }
        size_t capacity() const { return N; }

        bool full()  const { return used_count >= N; }
        bool empty() const { return used_count == 0; }

        T* front()
        {
            return empty() ? nullptr : &memory[bottom];
        }

        const T* front() const
        {
            return empty() ? nullptr : &memory[bottom];
        }

        T* back()
        {
            return empty() ? nullptr : &memory[__index(used_count - 1)];
        }

        const T* back() const
        {
            return empty() ? nullptr : &memory[__index(used_count - 1)];
        }

        void pop_front()
        {
            if (!empty())
            {
                bottom = __next(0);
                --used_count;
            }
        }

        int push_back(const T& t, bool replace = false)
        {
            if (full())
            {
                if (replace)
                    pop_front();
                else
                    return -ENOMEM;
            }

            memory[__index(used_count)] = t;
            used_count++;
            return 0;
        }

        T* at(size_t index)
        {
            CHECK_LOG_RETVAL(index < used_count, nullptr);

            return &memory[__index(index)];
        }

        const T* at(size_t index) const
        {
            CHECK_LOG_RETVAL(index < used_count, nullptr);

            return &memory[__index(index)];
        }

        template<typename... Args>
        T* emplace(Args&&... args)
        {
            if (full())
                return nullptr;

            T* t = &memory[__index(used_count)];
            new (t) T(std::forward<Args>(args)...);
            used_count++;

            return t;
        }

        iterator begin() { return iterator(this, 0); }
        iterator end() { return iterator(this, used_count); }
        const_iterator begin() const { return const_iterator(this, 0); }
        const_iterator end()   const { return const_iterator(this, used_count); }
    };
}