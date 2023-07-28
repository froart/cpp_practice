#include <iostream>
#include <limits>

using namespace std;

class CustomException : public exception {
	private:
		string msg_;
	public:
		CustomException(string msg) : msg_(msg) {};
		string what () {
			return msg_;
  	};
};

float divide(float a, float b) {
	if (b == 0) {
		 throw CustomException("Division by zero!");
  }
	return a/b;
}

int main() {

	float a = 5.0;
	float b = 0.0;
	float r;
	try { 
		r = divide(a, b); 
	} catch(CustomException& ex) { 
		cerr << ex.what() << endl;	
    r = numeric_limits<float>::infinity();
	}

	cout << "a / b = " << r << endl;
	return 0;
}
