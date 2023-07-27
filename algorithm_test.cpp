#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <cmath>

using namespace std;

int main() {

	vector<int> vec1(20);
  // vector elements initialization method #1
	for(int i = 1; auto& e : vec1) e = pow(2,i++);	
  // display vector
  for(const auto& e : vec1) cout << e << " "; 
  cout << endl;

  vector<int> vec2(20);
  // vector elements initialization method #2
	for_each(vec2.begin(), vec2.end(), [](auto& e){ static int i = 0; e = pow(2,i++); }); // array initialization
  // display vector
	for(auto& e : vec2) cout << e << " "; 
  cout << endl;

	if(all_of(vec1.begin(), vec1.end(), [](int& e){ return e%2 == 0;})) // check if all elements of the array are even 
		cout << "all vector #1 elements are even" << endl;
	else cout << "not all vector #1 elements are even" << endl;

	if(all_of(vec2.begin(), vec2.end(), [](int& e){ return e%2 == 0;})) // check if all elements of the array are even 
		cout << "all vector #2 elements are even" << endl;
	else cout << "not all vector #2 elements are even" << endl;
 
	return 0;
}
