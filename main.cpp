

#include <stdio.h>
#include "memsafe.hpp"


using namespace memsafe;


struct A : public MemSafeObj {
	int i = 0;
};


void mem_ok() {
	if (true) {
		m_<A> a(new A());
		delete_it(a);
	}
	if (true) {
		A a;
		m_<A> p(a);
	}
}

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
		default:
			goto end;
			break;
		}
	}

end:
	puts("END");
	return 0;
}
