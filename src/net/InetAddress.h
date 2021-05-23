#ifndef __MUTTY_INET_ADDRESS_H__
#define __MUTTY_INET_ADDRESS_H__
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include "utils/StringUtils.h"
namespace mutty {

struct InetAddress {
public:
    InetAddress(sockaddr_in address = {}): _address(address) {}
    InetAddress(uint32_t ip, uint16_t port): _address{AF_INET, ::htons(port), ::htonl(ip)} {}
    InetAddress(const std::string &ip, uint16_t port): InetAddress(stringToIp(ip), port) {}
    InetAddress(const std::string &address);

    std::string toString() { return ipToString() + ":" + std::to_string(::ntohs(_address.sin_port)); }

private:
    std::string ipToString();
    uint32_t stringToIp(std::string s); // static

    sockaddr_in _address;
};

inline InetAddress::InetAddress(const std::string &address) {
    auto pivot = split(address, ':');
    assert(pivot.size() == 2);
    _address = {
        AF_INET, 
        ::htons(toDec<uint16_t>(address.substr(pivot[1].first, pivot[1].second - pivot[1].first))),
        ::htonl(stringToIp(address.substr(pivot[0].first, pivot[0].second - pivot[0].first)))
    };
}

inline std::string InetAddress::ipToString() {
    std::string s;
    uint32_t addr = ::ntohl(_address.sin_addr.s_addr); 
    for(int chunk = 3; ~chunk; --chunk) {
        s.append(".", 3 > chunk).append(std::to_string((addr >> (chunk << 3)) & 0xff));
    }
    return s;
}

inline uint32_t InetAddress::stringToIp(std::string s) {
    auto pivots = split(s, '.');
    uint32_t ip = 0;
    for(auto &&p : pivots) {
        ip = (ip << 8) | toDec<uint32_t>(s.substr(p.first, p.second-p.first));
    }
    return ip;
}

} // mutty

#endif