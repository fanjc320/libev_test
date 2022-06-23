#pragma once

namespace fix
{
    template<size_t N>
    struct fix_string
    {
        size_t used_count = 0;
        char memory[sizeof(char) * N] = {};

        fix_string(){}
        ~fix_string() { clear(); }

        explicit fix_string(const char* str)
        {
            used_count = copy(this->memory, N, str, strlen(str));
        }

        explicit fix_string(const std::basic_string<char>& str)
        {
            used_count = copy(this->memory, N, str.c_str(), str.length());
        }

        template<size_t M>
        fix_string(const fix_string<M>& str)
        {
            used_count = copy(this->memory, N, str.memory, str.length());
        }

        fix_string& operator=(const char* str)
        {
            used_count = copy(this->memory, N, str, strlen(str));
            return *this;
        }

        template<size_t M>
        fix_string& operator=(const fix_string<M>& str)
        {
            used_count = copy(this->memory, N, str.memory, str.length());
            return *this;
        }

        bool operator==(const char* str) const
        {
            return eq(memory, used_count, str, strlen(str));
        }

        bool operator!=(const char* str) const
        {
            return !(this->operator ==(str));
        }

        template<size_t M>
        bool operator==(const fix_string<M>& str) const
        {
            return eq(memory, used_count, str.memory, str.used_count);
        }

        template<size_t M>
        bool operator!=(const fix_string<M>& str) const
        {
            return !(this->operator ==(str));
        }

        void clear()
        {
            used_count = 0;
            memory[0] = '\0';
        }

        size_t fill(char* array, size_t capacity)
        {
            return copy(array, capacity, memory, used_count);
        }

        const char* c_str() const { return memory; }
        operator std::string() const { return { memory, used_count }; }

        size_t length()   const { return used_count; }
        size_t capacity() const { return N; }

        // we have a '\0' at the end, so used_count is compared to N - 1 here
        bool full()  const { return used_count >= N - 1; }
        bool empty() const { return used_count <= 0; }

        static size_t copy(char* dst, size_t dst_capacity,
            const char* src, size_t src_length)
        {
            CHECK_LOG_RETVAL(dst_capacity > 0, 0);

            size_t len = min(dst_capacity - 1, src_length);
            if (len > 0)
                memcpy(dst, src, len);

            dst[len] = '\0';
            return len;
        }

        static size_t strlen(const char* str)
        {
            if (!str) return 0;

            size_t i = 0;
            while (str[i] != '\0') ++i;

            return i;
        }

        static bool eq(const char* str1, size_t length1,
            const char* str2, size_t length2)
        {
            if (length1 != length2)
                return false;

            for (size_t i = 0; i < length1; ++i)
                if (str1[i] != str2[i])
                    return false;

            return true;
        }
    };
}