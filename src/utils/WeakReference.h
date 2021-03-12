#ifndef __UTILS_WEAK_REFERENCE_H__
#define __UTILS_WEAK_REFERENCE_H__
#include <bits/stdc++.h>
namespace mutty {

// GC interface with CRTP style
// should implement reusable() / isResuable() / get()
template <typename T, size_t step = 2>
class WeakReference {
public:
    WeakReference(): _reusableIndex(0), _window{0, -1} {}

    // 一个GC接口
    // 通过维护一个在[reusableIndex, container.size())的全nullptr的区间来减少resize的可能性
    // 单次操作保证O(1)，而非全部均摊O(1)，用于减少内存碎片和尽可能避免vector内部capacity扩大的情况，且需要额外的空间为O(1)
    void updateReusableIndex() {
        auto resetWindow = [this] {
            _window.left = 0;
            _window.right = -1;
        };
        if(_reusableIndex != 0) { // need GC
            for(int _ = 0; _ < step; ++_) {
                if(_window.left > _window.right) { // 未启动
                    if(static_cast<T*>(this)->isResuable(_window.left)) { // 可以启动
                        ++_window.right;
                    } else { // 零大小窗口整体右滑，仍未启动
                        ++_window.left;
                        ++_window.right;
                        if(_window.left == _reusableIndex) { // TODO 没有准确算过
                            resetWindow();
                        }
                    }
                    continue;
                }
                // 直到_right >= _left 说明container[left]是一个nullptr

                // left/right < reuseIndex
                if(_window.right + 1 == _reusableIndex) { // merge, [left,right] + [reuse,size()) 全是nullptr，可回收
                    _reusableIndex = _window.left;
                    resetWindow(); // close window
                } else if(static_cast<T*>(this)->isResuable(_window.right + 1)) {
                    ++_window.right;
                } else { // 没有触发到reuse，但是找到一个可用连接
                    std::swap(static_cast<T*>(this)->get(++_window.right),
                              static_cast<T*>(this)->get(_window.left++)); // R+1 和 L交换，且窗口右滑
                }
                    
            }
        }
    }

public:
    int _reusableIndex; // 第一个可用的index
    struct { int left, right; } _window; // 用于维护回收的窗口
};

} // mutty
#endif