/*
 * Copyright (c) 2022, chunquedong
 *
 * This file is part of cppfan project
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE version 3.0
 *
 * History:
 *   2022-10-12  Jed Young  Creation
 */

#ifndef _CPPF_UNSAFE_H_
#define _CPPF_UNSAFE_H_

#include <vector>

namespace memsafe {

thread_local std::vector<const char*> _unsafe_stack;

#define _sl_(x) #x
#define _strline_(x) _sl_(x)
#define UNSAFE(ptr, name) memsafe::UnsafeGuard _unsafe_gurad_ ## ptr ## name (__FUNCTION__); ptr._func = __FUNCTION__;

void my_assert(bool c, const char* msg);

struct UnsafeGuard {
    UnsafeGuard(const char* func) {
        _unsafe_stack.push_back(func);
    }

    ~UnsafeGuard() {
        _unsafe_stack.pop_back();
    }
};

/**
* unique ownership pointer
*/
template<typename T>
class UnsafePtr {
    template<typename X> friend void delete_unsafe_ptr(X& ptr);

    T* pointer;
public:
#if _DEBUG
    const char* _func = nullptr;
#endif
public:
    UnsafePtr() : pointer(nullptr) {
    }

    explicit UnsafePtr(T* p) : pointer(p) {
    }

    UnsafePtr(const UnsafePtr& other) : pointer(p) {
        check_unsafe();
    }

    UnsafePtr& operator=(const UnsafePtr& other) {
        check_unsafe();
        this->pointer = other.pointer;
        return *this;
    }

    T* operator->() { check_unsafe(); my_assert(pointer != nullptr, "try deref null pointer"); return pointer; }

    T& operator*() { check_unsafe(); my_assert(pointer != nullptr, "try deref null pointer"); return *pointer; }

    operator T*() { return pointer; }

private:
#if _DEBUG
    void check_unsafe() {
        my_assert(_unsafe_stack.size() > 0, "must define UNSAFE");
        my_assert(_func != nullptr, "must define UNSAFE");
        my_assert(strcmp(_unsafe_stack.back(), _func) == 0, "must define UNSAFE");
    }
#else
    void check_unsafe() {}
#endif
};

template<typename T>
void delete_unsafe_ptr(T& ptr) {
    ptr.check_unsafe();
    delete ptr.pointer;
    ptr.pointer = nullptr;
}

template<typename T>
using unsafe_ = UnsafePtr<T>;

}//namespace
#endif