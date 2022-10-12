/*
 * Copyright (c) 2022, chunquedong
 *
 * This file is part of cppfan project
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE version 3.0
 *
 * History:
 *   2022-10-12  Jed Young  Creation
 */

#ifndef _CPPF_UNIQUE_H_
#define _CPPF_UNIQUE_H_

#include <vector>

namespace memsafe {

void my_assert(bool c, const char* msg);

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

    operator T*() { return pointer; }
};


template<typename T, bool nullable = false>
using u_ = UniquePtr<T, nullable>;

}//namespace
#endif