#include <iostream>
#include <vector>

using namespace std;

template <class ItT>
void doubleInPlace(ItT begin, ItT end) {
	for(auto p = begin; p != end; ++p)
		*p *= 2;
}

int main() {
	vector<float> v(100, 2.0);
	for(auto i = v.begin(); i != v.end(); ++i) // i is a pointer to an element, so you should dereference it to get an element
		cout << *i << " "; 
  cout << endl;
	doubleInPlace(v.begin(), v.end());
	for(const auto& e : v) cout << e << " ";	
  cout << endl;
	return 0;
}
