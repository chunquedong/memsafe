

#include <stdio.h>
#include "memsafe.hpp"

using namespace memsafe;


struct A : public ManualObj {
	int i = 0;
};


void mem_ok() {
	if (true) {
		m_<A> a(new A());
		delete_manual_ptr(a);
	}
	if (true) {
		A a;
		m_<A> p(a);
	}
	if (true) {
		unsafe_<A> p(new A());
		UNSAFE(p);
		UNSAFE(p, 2);
		p = unsafe_<A>(new A());
		int i = p->i;
		delete_unsafe_ptr(p);
	}
}

void mem_leak() {
	m_<A> a(new A());
	//delete_manual_ptr(a);
}

void use_after_free() {
	m_<A> a(new A());
	delete_manual_ptr(a);
	int i = a->i;
}

void delete_alive() {
	m_<A> a(new A());
	m_<A> b = a;
	delete_manual_ptr(a);
}

void stack_dangling() {
	m_<A> p;
	if (true) {
		A a;
		p = m_<A>(a);
	}
}

void non_nullable() {
	m_<A> p(new A());
	m_<A> np;
	p = np;
}

void test_unique() {
	u_<A> p(new A());
	u_<A> p2 = std::move(p);
	int i = p->i;
}

void test_unsafe() {
	unsafe_<A> p(new A());
	//p = unsafe_<A>(new A());
	int i = p->i;
}

/**
 * execute the test.
 *
 */
int main(int argc, char **argv) {
	mem_ok();

	while (true) {
		int i = getchar();
		switch (i)
		{
		case '0':
			mem_leak();
			break;
		case '1':
			stack_dangling();
			break;
		case '2':
			delete_alive();
			break;
		case '3':
			use_after_free();
			break;
		case '4':
			non_nullable();
			break;
		case '5':
			test_unique();
			break;
		case '6':
			test_unsafe();
			break;
		default:
			goto end;
			break;
		}
	}

end:
	puts("END");
	return 0;
}
