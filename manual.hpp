/*
 * Copyright (c) 2022, chunquedong
 *
 * This file is part of cppfan project
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE version 3.0
 *
 * History:
 *   2022-10-1  Jed Young  Creation
 */

#ifndef _CPPF_MEMSAFE_H_
#define _CPPF_MEMSAFE_H_

#include <atomic>
#include <math.h>
#include <stdio.h>

namespace memsafe {


void my_assert(bool c, const char *msg = "") {
    if (!c) {
        printf("ERROR: %s\n", msg);
        abort();
    }
}

/**
* Base class for all memory safe class.
*/
class ManualObj {
    template<typename T> friend class ManualPtr;
    template<typename T> friend void delete_manual_ptr(T& ptr);
#if _DEBUG
    std::atomic<int> _ref_count;

public:
    ManualObj() {
    }

    virtual ~ManualObj() {
        my_assert(_ref_count == 0, "try delete a living pointer");
    }

private:

    void add_ref() {
        ++_ref_count;
    }

    void release(bool check, bool is_stack = false) {
        int rc = --_ref_count;
        if (check) {
            if (is_stack) {
                my_assert(rc >= 0, "memory leak");
            }
            else {
                my_assert(rc > 0, "memory leak");
            }
        }
    }

    int ref_count() { return _ref_count; }
#endif
};



/**
* Intrusive smart pointer. the T must be extend from MemSafeObj
*/
template<typename T>
class ManualPtr {
    template<typename X> friend void delete_manual_ptr(X& ptr);
    T* pointer;
#if _DEBUG
    bool in_stack;
    bool nullable;
public:
    ManualPtr() : pointer(nullptr), in_stack(false), nullable(true) {
    }

    explicit ManualPtr(T& p, bool is_nullable = false, bool in_stack = true)
        : pointer(&p), in_stack(in_stack), nullable(is_nullable) {
        p.add_ref();
    }

    explicit ManualPtr(T* p, bool is_nullable = false, bool in_stack = false) 
        : pointer(p), in_stack(in_stack), nullable(is_nullable) {
        if (p) {
            p->add_ref();
        }
        if (!nullable) {
            my_assert(p, "non-nullable");
        }
    }

    ~ManualPtr() {
        if (pointer) pointer->release(true, in_stack);
    }

    ManualPtr(const ManualPtr& other) {
        if (other.pointer) {
            other.pointer->add_ref();
        }
        nullable = other.nullable;
        in_stack = other.in_stack;
        pointer = other.pointer;
    }

    ManualPtr& operator=(const ManualPtr& other) {
        if (other.pointer) {
            other.pointer->add_ref();
        }
        else {
            if (!nullable) {
                my_assert(false, "non-nullable");
            }
        }

        if (pointer) {
            pointer->release(true, in_stack);
        }
        in_stack = other.in_stack;
        pointer = other.pointer;
        return *this;
    }
#else
public:
    ManualPtr() : pointer(nullptr) {}
    explicit ManualPtr(T* p, bool is_nullable = false, bool in_stack = false)
        : pointer(p) {
        if (!is_nullable) {
            my_assert(p, "non-nullable");
        }
    }
#endif
    T* operator->() { my_assert(pointer != nullptr, "try deref null pointer"); return pointer; }

    T& operator*() { my_assert(pointer != nullptr, "try deref null pointer"); return *pointer; }

    T* get() { return pointer; }

};

template<typename T>
void delete_manual_ptr(T& ptr) {
    ptr.pointer->release(false);
    delete ptr.pointer;
    ptr.pointer = nullptr;
}



template<typename T>
using m_ = ManualPtr<T>;



}//namespace
#endif
