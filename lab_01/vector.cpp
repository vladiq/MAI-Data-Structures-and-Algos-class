template<class T>
void VectorCreate(TVector<T> &v, size_t size) {
    v.data = new T[size + 1];
    v.cap = size + 1;
    v.size = size;
}

template<class T>
void VectorPushBack(TVector<T> &v, KV value) {
    if (v.size == v.cap) {
        v.cap *= 2;
        T *v2 = new T[v.cap];

        for (size_t i = 0; i < v.size; ++i) {
            v2[i] = v.data[i];
        }

        delete[] v.data;
        v.data = v2;
    }
    
    v.data[v.size++] = value;
}

template<class T>
void VectorDestroy(TVector<T> &v) {
    delete[] v.data;
    v.data = NULL;
    v.size = 0;
    v.cap = 0;
}

template<class T>
void VectorSwap(T &v1, T &v2) {
    T v3 = v1;
    v1 = v2;
    v2 = v3;
}
