#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <bits/stdc++.h>
#include "utils/Algorithms.h"
// 一个简单的Socket IO Buffer
// 提供char*兼容，以及尽可能延迟扩容
class Buffer {
public:
    Buffer(int size = 128): _size(size), _buf(size), _r(0), _w(0) { }
    
private:
    std::vector<char> _buf;
    int _size;  // ==capacity
    int _r, _w; // _r >= _w

public:

    void clear() {
        _r = _w = 0;
        // TODO 用_size控制吧
    }

    int size() {
        return _size;
    }

    // 还有多少未读取
    int rest() {
        return _r - _w;
    }


    // 还有多少彻底填满
    int limit() {
        return _size - _r;
    }

    int freeSpace() {
        return _size - rest();
    }

    void reuse() {
        _r = _w = 0;
    }

    // 能否复用，尽量延迟resize
    // TODO 当rest()为一个极少占比的时候，尝试copy
    void reuseIfPossible() {
        if(_r == _w) {
            reuse();
        }
    }

    // 处理边缘碎片
    void gc() {
        if(_w > 0) {
            // move [_w, _r] to front
            memmove(_buf.data(), _buf.data() + _w, _r = rest());
            _w = 0;
        }
        
    }


    // lazy resize if it can
    void gc(int hint) {
        if(hint <= limit()) return;
        gc();
        // still cannot store
        if(hint > freeSpace()) {
            // TODO expand    exactly or powerOf2?
            int appendSize = freeSpace() - hint;
            int expectSize = _buf.size() + appendSize;
            expandTo(_size = roundToPowerOfTwo(expectSize));
        }
    }

    void expand() {
        _buf.resize(_size = (_size << 1));
    }



    void expandTo(int size) {
        _buf.resize(_size = size);
    }

    void read(int n) { 
        // if
        _r += n; 
    }
    void write(int n) {  // 实际write后调用
        _w += n; 
        reuseIfPossible();
    }

    char* getReadBuffer() {
        return _buf.data() + _r;
    }

    char* getWriteBuffer() {
        return _buf.data() + _w;
    }

    char* end() {
        return _buf.data() + _buf.size();
    }

    

    void append(const char *data, int size) {
        gc(size);
        std::copy(data, data + size, getReadBuffer());
        _r += size;
    }

    // not a pointer
    template <typename T>
    void append(const T &data) {
        append(static_cast<const char *>(&data), sizeof(T));
    }
    


};
#endif