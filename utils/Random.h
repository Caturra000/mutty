#ifndef __UTILS_RANDOM_H__
#define __UTILS_RANDOM_H__
// 实现一个线程安全的简易随机数生成器
template <typename T = unsigned long long, unsigned long long init = 19260817>
inline T random() {
    static thread_local auto seed = init;
    seed = seed * 998244353 + 12345;
    return static_cast<T>(seed / 1024);
}
#endif