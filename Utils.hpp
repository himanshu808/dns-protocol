#ifndef UTILS_H
#define UTILS_H

#include "DnsHeader.hpp"

template<class T>
T enumFromIdx(unsigned i) {
    return static_cast<T>(i);
}

template<class T>
unsigned idxFromEnum(T t) {
    return T(t);
}

#endif