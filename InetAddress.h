#ifndef __INET_ADDRESS_H__
#define __INET_ADDRESS_H__

#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include "utils/StringUtils.h"
// 对地址类的浅层封装，要求大小与sockaddr_in一致 / 内存顶部是sockaddr_in
struct InetAddress {
public:
    InetAddress(sockaddr_in address): _address(address) { } // POD不需要move 且成本低不需要const&
    InetAddress(uint32_t ip, uint16_t port): _address{AF_INET, htons(port), htonl(ip)} { } // TODO 参数需要using隐藏
    InetAddress(const std::string &address) {
        auto pivot = split(address, ':');
        assert(pivot.size() == 2);
        InetAddress(stringToIp(address.substr(pivot[0].first, pivot[0].second - pivot[0].first)), 
                    toDec<uint16_t>(address.substr(pivot[1].first, pivot[1].second - pivot[1].first)));
    } 
    std::string toString() { return ipToString() + ":" + std::to_string(_address.sin_port); } // "192.168.0.1:2333"

private:

    // 实测toString/toIp是正确的，但是在InetAddress中需要考虑网络字节序

    // 别问为什么，我故意写的难看懂
    std::string ipToString() {
        std::string s;
        uint32_t addr = _address.sin_addr.s_addr; 
        for(int chunk = 3; ~chunk; --chunk) {
            s.append(".", 3 > chunk).append(std::to_string((addr >> (chunk << 3)) & 0xff));
        }
        return s;
    }
    
    uint32_t stringToIp(std::string s) {
        auto pivots = split(s, '.');
        uint32_t ip = 0;
        for(auto &&p : pivots) {
            ip = (ip << 8) | toDec<uint32_t>(s.substr(p.first, p.second-p.first));
        }
        return ip;
    }
    
    
    sockaddr_in _address; // 必须在顶部，保证可直接转型
    
};



#endif