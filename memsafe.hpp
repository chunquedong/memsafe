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
class MemSafeObj {
    template<typename T> friend class ManualPtr;
    template<typename T> friend void delete_it(T& ptr);
#if _DEBUG
    std::atomic<int> _ref_count;

public:
    MemSafeObj() {
    }

    virtual ~MemSafeObj() {
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
    template<typename X> friend void delete_it(X& ptr);
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
void delete_it(T& ptr) {
    ptr.pointer->release(false);
    delete ptr.pointer;
    ptr.pointer = nullptr;
}


/**
* unique ownership pointer
*/
template<typename T, bool nullable = false>
class UniquePtr {
    T* pointer;
public:
    UniquePtr() : pointer(nullptr) {
        if (!nullable) static_assert(false, "non-nullable");
    }

    explicit UniquePtr(T* p) : pointer(p) {
        if (!nullable) {
            my_assert(p, "non-nullable");
        }
    }

    ~UniquePtr() {
        delete pointer;
    }

    UniquePtr(const UniquePtr& other) = delete;

    UniquePtr(UniquePtr&& other) {
        if (!nullable) {
            my_assert(other.pointer, "non-nullable");
        }
        if (other.pointer) {
            pointer = other.pointer;
            other.pointer = nullptr;
        }
        else {
            pointer = nullptr;
        }
    }

    UniquePtr& operator=(const UniquePtr& other) = delete;

    UniquePtr& operator=(UniquePtr&& other) {
        if (!nullable) {
            my_assert(other.pointer, "non-nullable");
        }
        if (pointer) {
            delete pointer;
        }

        if (other.pointer) {
            pointer = other.pointer;
            other.pointer = nullptr;
        }
        return *this;
    }

    T* operator->() { my_assert(pointer != nullptr, "try deref null pointer"); return pointer; }

    T& operator*() { my_assert(pointer != nullptr, "try deref null pointer"); return *pointer; }

    operator T() { return pointer; }
};



template<typename T>
using m_ = ManualPtr<T>;

template<typename T, bool nullable = false>
using u_ = UniquePtr<T, nullable>;

}//namespace
#endif
