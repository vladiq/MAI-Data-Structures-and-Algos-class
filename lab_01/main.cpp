#include <iostream>

#include "vector.hpp"
#include "sort.hpp"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    TVector<KV> elems;
    KV data;
    TKey maxKey = 0;
    
    VectorCreate<KV>(elems, 0);
    
    while (std::cin.getline(data.value, 256)) {
        data.key = 0;

        if (data.value[0] == '\0') {
            continue;
        }

        Parsing(data);

        if (data.key > maxKey) {
            maxKey = data.key;
        }
        VectorPushBack<KV>(elems, data);
    }

    RadixSort(elems, maxKey);

    for (size_t i = 0; i < elems.size; i++) {
        std::cout << elems.data[i].value << std::endl;
    }

    VectorDestroy(elems);
    return 0;
}