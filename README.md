# memsafe

The real-time C++ memory safe check. No runtime overhead in release mode.

```
struct A : public MemSafeObj {
    int i = 0;
};

void mem_leak() {
    m_<A> a(new A());
    //delete_it(a);
}

void use_after_free() {
    m_<A> a(new A());
    delete_it(a);
    int i = a->i;
}

void delete_alive() {
    m_<A> a(new A());
    m_<A> b = a;
    delete_it(a);
}

void stack_dangling() {
    m_<A> p;
    if (true) {
        A a;
        p = m_<A>(a);
    }
}
```
