#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstdlib>

typedef unsigned long long TKey;
typedef char TValue;

struct KV {
    TKey key;
    TValue value[256];
};

template<class T>
struct TVector {
    T *data;
    size_t size;
    size_t cap;
};

template<class T>
void VectorCreate(TVector<T> &v, size_t size);

template<class T>
void VectorPushBack(TVector<T> &v, KV value);

template<class T>
void VectorDestroy(TVector<T> &v);

template<class T>
void VectorSwap(T &a, T &b);

#include "vector.cpp"

#endif
