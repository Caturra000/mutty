#ifndef __CONNECTION_POOL_H__
#define __CONNECTION_POOL_H__
#include<bits/stdc++.h>
#include"TcpHandler.h"
// 用于为Server提供Connection容器
class ConnectionPool {

private:
    std::map<int, std::unique_ptr<TcpHandler>> _container;
};
#endif