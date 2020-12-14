#ifndef __UTILS_STRING_UTILS_H__
#define __UTILS_STRING_UTILS_H__
#include <bits/stdc++.h>


// 只能用于文本，二进制流不兼容
std::vector<std::pair<int, int>>
split(const char *str, const char pivot = ' ') {
    std::vector<std::pair<int, int>> result;
    int lo = 0, hi = 0;
    while(*str) {
        while(*str && *str == pivot) ++str, ++lo, ++hi;
        while(*str && *str != pivot) ++str, ++hi;
        if(lo < hi) result.emplace_back(lo, hi);
        lo = hi;
    }
    return result;
}

std::vector<std::pair<int, int>>
split(const char *str, int length, const char pivot = ' ') {
    std::vector<std::pair<int, int>> result;
    int lo = 0, hi = 0;
    while(length) {
        while(length && *str == pivot) ++str, ++lo, ++hi, --length;
        while(length && *str != pivot) ++str, ++hi, --length;
        if(lo < hi) result.emplace_back(lo, hi);
        lo = hi;
    }
    return result;
}

std::vector<std::pair<int, int>>
split(const std::string &str, const char pivot = ' ') {
    return split(str.c_str(), str.length(), pivot);
}

template<typename T>
T toDec(const std::string &str) {
    static_assert(std::is_integral<T>::value, "T must be integral");
    T dec = 0;
    for(auto c : str) {
        dec = dec*10 + (c - '0');
    }
    return dec;
}

// stack
template <size_t N>
class FastIo {
public:
    std::pair<const char*, size_t> getline(std::istream &in = std::cin) {
        if(cur > N) clear();
        in.getline(_buf + cur, M - cur);
        size_t bound = strlenFast2();
        auto result = std::make_pair(_buf + cur, bound - cur);
        cur = bound;
        return result;
    }

private:
    void clear() {
        memset(_buf, 0, cur);
        cur = 0;
    }

    // 求的是绝对值
    size_t strlenFast() {
        size_t lo = cur, hi = M-1;
        while(lo < hi) {
            size_t mid = lo + (hi-lo >> 1);
            if(_buf[mid] == '\0') hi = mid;
            else lo = mid+1;
        }
        return lo; // 第一个为'\0'的pos
    }

    size_t strlenFast2() {
        size_t lo = cur >> 3;
        size_t hi = M-1 >> 3;
        while(lo < hi) {
            size_t mid = lo + (hi-lo >> 1);
            auto chars = *((long long*)(_buf + (mid << 3)));
            if((chars & 0xff) == 0) hi = mid;
            else lo = mid + 1;
        }
        lo = ((lo ? lo-1 : lo) << 3);
        for(int i = lo, j = lo + 8; i < j; ++i) {
            if(_buf[i] == '\0') return i;
        }
        return M-1;
    }

    static constexpr size_t M  = ((N+3 >> 2) << 3) + 8;
    char _buf[M] {};
    size_t cur = 0;
};

#endif