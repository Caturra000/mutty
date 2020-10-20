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
        auto connection = std::make_unique<TcpHandler>(
                _looperPool.pick().get(), 
                std::move(acceptedsocket), localAddress, peerAddress); 
        if(!_reuseableIndex.empty()) {
            int pos = _reuseableIndex.back();
            _container[pos] = std::move(connection);
            _reuseableIndex.pop_back();
            return _container[pos];
        } else {
            _container.emplace_back(std::move(connection));
            return _container.back();
        }
    }
private:
    // std::map<int, std::unique_ptr<TcpHandler>> _container;

    std::vector<std::unique_ptr<TcpHandler>> _container;
    std::vector<int> _reuseableIndex; // TODO reuse策略 目前没解决碎片问题 给出简单均摊O(1)的倍增gc也可以
    LooperPool<1<<4> _looperPool;
};
#endif