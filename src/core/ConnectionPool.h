#ifndef __MUTTY_CONNECTION_POOL_H__
#define __MUTTY_CONNECTION_POOL_H__
#include <bits/stdc++.h>
#include "utils/Compat.h"
#include "utils/WeakReference.h"
#include "utils/NonCopyable.h"
#include "TcpBridge.h"
#include "LooperPool.h"
namespace mutty {

template <size_t N>
class ConnectionPoolBase
    : private WeakReference<ConnectionPoolBase<N>>,
      private NonCopyable {
public:

    std::shared_ptr<TcpContext> createNewConnection(Socket acceptedsocket,
                                                    InetAddress localAddress,
                                                    InetAddress peerAddress);

    void stop();

    ConnectionPoolBase(int size = 16): _container(size) {}
    ~ConnectionPoolBase() { if(!_stop) stop(); }

private:
    std::shared_ptr<TcpContext>& get(int index) { return _container[index]; }
    bool reusable() { return GcBase::_reusableIndex != _container.size(); }
    bool isResuable(int index);

private:
    std::vector<std::shared_ptr<TcpContext>> _container;
    LooperPool<N> _looperPool;
    bool _stop = false;

    using GcBase = WeakReference<ConnectionPoolBase<N>>;
    friend class WeakReference<ConnectionPoolBase<N>>;
};

using ConnectionPool = ConnectionPoolBase<1<<4>;
using SingleConnectionPool = ConnectionPoolBase<1>;

template <size_t N>
inline std::shared_ptr<TcpContext> ConnectionPoolBase<N>::createNewConnection(
        Socket acceptedsocket, InetAddress localAddress, InetAddress peerAddress) {
    if(_container.size() > 128) GcBase::updateReusableIndex();
    auto connection = std::make_shared<TcpContext>(
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
inline void ConnectionPoolBase<N>::stop() {
    for(auto &connection : _container) {
        if(connection == nullptr) continue; // killed by GcBase
        connection->forceClose();
    }
    for(size_t i = 0; i < N; ++i) {
        _looperPool.pick(i)->stop();
    }
    _stop = true;
}

template <size_t N>
inline bool ConnectionPoolBase<N>::isResuable(int index) {
    auto &connection = _container[index];
    return connection == nullptr || connection->isDisConnected();
}

} // mutty
#endif