//
// Created by evcmo on 6/1/2021.
//
#pragma once
#ifndef CHARON_LIST_H
#define CHARON_LIST_H
#include <cassert>
#include <memory>
#include <iostream>

using std::make_shared;
using std::shared_ptr;
using std::make_unique;
using std::unique_ptr;

template<typename T>
class List {
public:
    virtual void add(T t) = 0;
    virtual void set(int i, T  t) = 0;
    virtual void insert(int i, T  t) = 0;
    [[nodiscard]] virtual int size() const = 0;
    [[nodiscard]] virtual bool isEmpty() const = 0;
    virtual T operator[](int i) const = 0;
    virtual ~List() = 0;
private:
    template<typename X>
    class ReadOnlyList;
};

template<typename T>
List<T>::~List() = default;

template<typename T> class ReadOnlyList;

template<typename T>
class ReadOnlyList<T*> final:
        public List<T*> {
private:
    const List<T*>* internal;
public:
    explicit ReadOnlyList(List<T*>*);
    void add(T*) override;
    void set(int, T*) override;
    void insert(int, T*) override;
    [[nodiscard]] int size() const override;
    [[nodiscard]] bool isEmpty() const override;
    T* operator[](int i) const override;
    explicit ReadOnlyList(const ReadOnlyList&) = delete;
    ~ReadOnlyList();
};

template <typename T>
ReadOnlyList<T*>::
ReadOnlyList(List<T*>* const sl) :
        internal(sl)
{  }

template <typename T>
void ReadOnlyList<T*>::
add(T* const t)
{ throw std::exception(); }

template <typename T>
void ReadOnlyList<T*>::
set(const int i, T* const t)
{ throw std::exception(); }

template <typename T>
void ReadOnlyList<T*>::
insert(const int i, T* const t)
{ throw std::exception(); }

template <typename T>
int ReadOnlyList<T*>::
size() const
{ return internal->size(); }

template <typename T>
bool ReadOnlyList<T*>::
isEmpty() const
{ return internal->isEmpty(); }

template<typename T>
T* ReadOnlyList<T*>::
operator[](const int i) const {
    assert(i >= 0 && i < internal->size());
    return (*internal)[i];
}

template<typename T>
ReadOnlyList<T*>::~ReadOnlyList() {
    delete internal;
}

template<typename T> class Lists;

template <typename T>
class Lists<T*> {
public:
    static List<T*>*
    uniqueReadOnly(List<T*>* t);
};

template<typename T>
List<T*>* Lists<T*>::
uniqueReadOnly(List<T*>* const t)
{ return new ReadOnlyList<T*>(t); }

#endif //CHARON_LIST_H

