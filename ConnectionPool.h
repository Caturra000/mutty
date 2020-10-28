#ifndef __CONNECTION_POOL_H__
#define __CONNECTION_POOL_H__
#include <bits/stdc++.h>
#include "TcpHandler.h"
#include "LooperPool.h"
// 用于为Server提供Connection容器
class ConnectionPool {
public:

    // 返回值只建议在调用该方法的最小作用域内使用
    std::unique_ptr<TcpHandler>& createNewConnection(
            Socket acceptedsocket, InetAddress localAddress, InetAddress peerAddress) {
        if(_container.size() > 128) updateReusableIndex();
        auto connection = std::make_unique<TcpHandler>(
                _looperPool.pick().get(),  // TODO looperThread
                std::move(acceptedsocket), localAddress, peerAddress); 
        if(reusable()) {
            int pos = _reusableIndex++;
            _container[pos] = std::move(connection);
            return _container[pos];
        } else {
            _container.emplace_back(std::move(connection));  // emplace_back后会满足reusable的情形
            return _container.back();
        }
    }

    bool reusable() { return _reusableIndex != _container.size(); }

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
                    if(_container[_window.left] == nullptr) { // 可以启动
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
                } else if(_container[_window.right + 1] == nullptr) {
                    ++_window.right;
                } else { // 没有触发到reuse，但是找到一个可用连接
                    std::swap(_container[++_window.right],
                         _container[_window.left++]); // R+1 和 L交换，且窗口右滑
                }
                    
            }
        }
    }

    ConnectionPool(int size = 16): _container(size), _reusableIndex(0), _window{0,-1} { }


private:
    std::vector<std::unique_ptr<TcpHandler>> _container;

    int _reusableIndex; // 第一个可用的index
    struct { int left, right; } _window; // 用于维护回收连接的窗口
    constexpr static int step = 2;

    LooperPool<1<<4> _looperPool;
};
#endif