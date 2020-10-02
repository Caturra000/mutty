#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <bits/stdc++.h>

class Buffer {
public:
    Buffer(int size): _size(size), _buf(size), _r(0), _w(0), _total(size) { }
    
private:
    std::vector<char> _buf;
    std::vector<char> _next;
    int _size, _total;
    int _r, _w;

    void read() { // 理应返回一对值 ===> 但是跨buffer了，草
        while(_r!=_w) _r++;
    }
    

};
#endif