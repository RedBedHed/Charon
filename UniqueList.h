//
// Created by evcmo on 6/1/2021.
//

#pragma once
#ifndef CHARON_UNIQUELIST_H
#define CHARON_UNIQUELIST_H

#include <ostream>
#include <cassert>
#include <vector>
#include <iostream>
#include "List.h"

using std::ostream;

template<typename T> class UniqueList;

template<typename T>
class UniqueList<T*> final : public List<T*> {
private:
    static constexpr int INITIAL_SIZE = 256;
    T** data;
    int runningSize;
    int fixedSize;
    void grow();
public:
    explicit UniqueList();
    explicit UniqueList(int);
    void add(T*) override;
    void set(int, T*) override;
    void insert(int, T*) override;
    [[nodiscard]] int size() const override;
    [[nodiscard]] bool isEmpty() const override;
    T* operator[](int) const override;
    friend ostream& operator<<(ostream& out, UniqueList& sl) {
        for(int i = 0; i < sl.runningSize; ++i)
            out << sl.data[i] << '\n';
        return out;
    }
    friend ostream& operator<<(ostream& out, UniqueList* sl) {
        for(int i = 0; i < sl->runningSize; ++i)
            out << sl->data[i] << '\n';
        return out;
    }
    ~UniqueList();
};

template<typename T>
UniqueList<T*>::UniqueList() {
    data = new T*[fixedSize = INITIAL_SIZE];
    runningSize = 0;
}

template<typename T>
UniqueList<T*>::
UniqueList(const int size) {
    assert(size > 0);
    data = new T*[fixedSize = size];
    runningSize = 0;
}

template<typename T>
void UniqueList<T*>::
add(T* const t) {
    grow();
    data[runningSize++] = t;
}

template<typename T>
inline void UniqueList<T*>::grow() {
    if(runningSize >= fixedSize) {
        auto* temp = new T*[fixedSize <<= 1U];
        for(int i = 0; i < runningSize; ++i)
            temp[i] = data[i];
        delete[] data;
        data = temp;
    }
}

template<typename T>
void UniqueList<T*>::
insert(const int i, T* const t) {
    assert(i >= 0 && i < runningSize);
    grow();
    for(int x = runningSize++; x >= i; --x)
        data[x + 1] = data[x];
    data[i] = t;
}

template<typename T>
void UniqueList<T*>::
set(const int i, T* const t) {
    assert(i >= 0 && i < runningSize);
    grow();
    data[i] = t;
}

template<typename T>
int UniqueList<T*>
::size() const {
    return runningSize;
}

template<typename T>
bool UniqueList<T*>::
isEmpty() const {
    return runningSize <= 0;
}

template<typename T>
T* UniqueList<T*>::
operator[](int i) const {
    assert(i >= 0 && i < runningSize);
    return data[i];
}

template<typename T>
UniqueList<T*>::~UniqueList() {
    for(int i = 0; i < runningSize; i++) {
        try { delete data[i]; }
        catch (std::exception& e)
        {  }
    }
    delete[] data;
}

#endif //CHARON_UNIQUELIST_H

