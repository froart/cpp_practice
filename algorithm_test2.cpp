#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;

int main(int argc, char** argv) {

   vector<int> v(20, 1);
   for_each(v.begin(), v.end(), 
           [](auto& e) { 
              static int i = 1; 
              e = pow(2,i++); 
           }
           );

   cout << "Elements of vector v:" << endl;
   for(const auto& e : v) cout << e << " ";
   cout << endl;
   if(all_of(v.begin(), v.end(), [](auto& e){ return e%2==0; }))
     cout << "all elements of vector v are even" << endl;
   else 
     cout << "not all elements of vector v are even" << endl;
  
   // find the first element which satisfies the condition
   vector<int>::iterator p = find_if(v.begin(), v.end(), [](int &a){ return a > 20;});
   cout << "The first element of the vector v which is greater than 20 is " << *p << endl;;

   vector<int> u(15, 0);
   // NOTE: changes the input vector too
   transform(v.begin(), v.end(), u.begin(), [](auto& e){ return e*=2; });
   cout << "Elements of vector u:" << endl;
   for(auto& e: u) cout << e << " ";
   cout << endl;

   sort(v.begin(), v.end(), greater<int>());
   cout << "Sorted elements of vector v:" << endl;
   for(auto& e: v) cout << e << " ";
   cout << endl;

   sort(u.begin(), u.end(), [](int& a, int& b){return a > b;}); 
   cout << "Sorted elements of vector u:" << endl;
   for(auto& e: u) cout << e << " ";
   cout << endl;


   return 0;
}
