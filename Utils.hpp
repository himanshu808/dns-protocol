#ifndef UTILS_H
#define UTILS_H

#include "DnsHeader.hpp"

inline template<class T>
T enumFromIdx(unsigned i) {
    return static_cast<T>(i);
}

inline template<class T>
unsigned idxFromEnum(T t) {
    return T(t);
}

#endif