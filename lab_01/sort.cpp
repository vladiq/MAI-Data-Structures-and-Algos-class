#include "sort.hpp"


bool IsDigit(char a) {
    if (a >= '0' && a <= '9') {
        return 1;
    }
    return 0;
}

void Parsing(KV& data) {
    for (int j = 0; data.value[j] != '\t'; ++j) {
            if (IsDigit(data.value[j])) {
                data.key = data.key * 10 + data.value[j] - '0';
            }
        }
}

void CountingSort(TVector<KV> &elems, unsigned long long div) {
    if (elems.size < 2) {
        return;
    }

    TVector<KV> result;
    VectorCreate<KV>(result, elems.size);

    size_t temp[10] = {0};

    for (size_t i = 0; i < elems.size; i++) {
        temp[(elems.data[i].key / div) % 10]++;
    }

    for (int i = 1; i < 10; i++) {
        temp[i] += temp[i - 1];
    }

    for (size_t i = elems.size ; i > 0; --i) {
        result.data[temp[(elems.data[i - 1].key / div) % 10] - 1] = elems.data[i - 1];
        temp[(elems.data[i - 1].key / div) % 10]--;
    }

    VectorSwap(elems, result);
    VectorDestroy(result);
}

void RadixSort(TVector<KV> &elems, TKey maxNum) {
    for(unsigned long long div = 1; maxNum / div > 0; div *= 10) {
        CountingSort(elems, div);
    }
}