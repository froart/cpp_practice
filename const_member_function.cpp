#include <iostream>
#include <cmath>

using namespace std;

template <typename T>
class Complex {
	T re;
	T im;
	public:
		Complex(T r, T i) : re(r), im(i) {};
		Complex() : re(0), im(0) {};
		T magnitude() const { return sqrt(re * re + im * im); }; // const means that this member function will not alternate the object state in any way
																														 // useful if an object is declared const => the magnitude will be able to be called
																														 // or else impossible!
		Complex operator+(const Complex& c2) const { return Complex(this->re + c2.re, this->im + c2.im); };
};

int main() {

	const Complex<float> c1(2.0, 1.0);
	const Complex<float> c2(2.0, 1.0);
	cout << "magnitude of a complex number c1 is: " << c1.magnitude() << endl;
	cout << "magnitude of a complex number c2 is: " << c2.magnitude() << endl;
  Complex<float> c3;
  c3 = c1 + c2;
	cout << "magnitude of a sum of c1 and c2 is: " << c3.magnitude() << endl;

	return 0;

}
