#ifndef SORT_HPP
#define SORT_HPP

#include "vector.hpp"

bool IsDigit(char a);
void Parsing(KV& data);
void CountingSort(TVector<KV> &elems, unsigned long long div);
void RadixSort(TVector<KV> &elems, TKey maxNum);

#include "sort.cpp"

#endif
