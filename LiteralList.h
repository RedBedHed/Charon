//
// Created by evcmo on 6/10/2021.
//

#ifndef CHARON_LITERALLIST_H
#define CHARON_LITERALLIST_H
#include "List.h"

template<typename T>
class LiteralList final : public List<T> {
private:
    const int fixedSize;
    int runningSize;
    const T data[];
public:
    constexpr LiteralList(const T d[], const int fixedSize) :
    fixedSize(fixedSize),
    runningSize(0),
    data(d)
    {  }

    inline constexpr void add(T t) {
        assert(runningSize < fixedSize);
        data[runningSize++] = t;
    }

    inline constexpr void set(const int i, T t) {
        assert(i >= 0 && i < fixedSize);
        data[i] = t;
    }

    inline constexpr void insert(const int i, T t) {
        assert(i >= 0 && i < fixedSize && runningSize < fixedSize);
        for(int x = runningSize++; x >= i; --x)
            data[x + 1] = data[x];
        data[i] = t;
    }

    inline constexpr T operator[](const int i) const {
        assert(i >= 0 && i < runningSize);
        return data[i];
    }

    [[nodiscard]]
    inline int size() const
    { return runningSize; };

    [[nodiscard]]
    inline constexpr bool isEmpty() const
    { return runningSize <= 0; }
};


#endif //CHARON_LITERALLIST_H
