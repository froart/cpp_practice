#include <iostream>
#include <vector>

using namespace std;

class Integer {
	public:
		Integer() {};
		virtual int put() = 0;
};

class Two : public Integer {
	public:
		 Two() {};
		 int put() { return 2; }
};

class One : public Integer {
	public:
		 One() {};
		 int put() { return 1; }
};


int main() {
	cout << "polymorphism using pointers:" << endl;
	Two t;
	One o;
	Integer* integ = &o;
	cout << integ->put() << " ";
	integ = &t;
	cout << integ->put() << " ";
  cout << endl;

	cout << "polymorphism using references:" << endl; // >>> doesn't work! not allowed to reassign reference <<<
	Two t2;
	One o2;
	Integer& integ2 = o2;
	cout << integ2.put() << " ";
	integ2 = t2;
	cout << integ2.put() << " ";
  cout << endl;
}
