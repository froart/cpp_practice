#include <iostream>
#include <cmath>

using namespace std;

namespace one {
	void func() { cout << "namespace one" << endl; }
}

namespace two {
	void func() { cout << "namespace two" << endl; }
}

namespace three {
	void func() { cout << "namespace three" << endl; }
}

int main () {
	
	one::func();
	two::func();
	three::func();

  return 0;
}
