#ifndef __MUTTY_CONNECTION_POOL_H__
#define __MUTTY_CONNECTION_POOL_H__
#include <bits/stdc++.h>
#include "utils/Compat.h"
#include "utils/WeakReference.h"
#include "TcpHandler.h"
#include "LooperPool.h"
namespace mutty {

template <size_t N>
class ConnectionPoolBase: private WeakReference<ConnectionPoolBase<N>> {
public:

    std::unique_ptr<TcpHandler>& createNewConnection(Socket acceptedsocket,
                                                     InetAddress localAddress,
                                                     InetAddress peerAddress);

    ConnectionPoolBase(int size = 16): _container(size) { }

private:
    std::unique_ptr<TcpHandler>& get(int index) { return _container[index]; }
    bool reusable() { return GcBase::_reusableIndex != _container.size(); }
    bool isResuable(int index);

private:
    std::vector<std::unique_ptr<TcpHandler>> _container;
    LooperPool<N> _looperPool;

    using GcBase = WeakReference<ConnectionPoolBase<N>>;
    friend class WeakReference<ConnectionPoolBase<N>>;
};

using ConnectionPool = ConnectionPoolBase<1<<4>;
using SingleConnectionPool = ConnectionPoolBase<1>;

template <size_t N>
inline std::unique_ptr<TcpHandler>& ConnectionPoolBase<N>::createNewConnection(
        Socket acceptedsocket, InetAddress localAddress, InetAddress peerAddress) {
    if(_container.size() > 128) GcBase::updateReusableIndex();
    auto connection = cpp11::make_unique<TcpHandler>(
            _looperPool.pick().get(),
            std::move(acceptedsocket), localAddress, peerAddress); 
    if(reusable()) {
        int pos = GcBase::_reusableIndex++;
        _container[pos] = std::move(connection);
        return _container[pos];
    } else {
        _container.emplace_back(std::move(connection));
        return _container.back();
    }
}

template <size_t N>
inline bool ConnectionPoolBase<N>::isResuable(int index) {
    auto &connection = _container[index];
    return connection == nullptr || connection->isContextDisconnected();
}

} // mutty
#endif