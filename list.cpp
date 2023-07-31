#include <iostream>
#include <vector>
#include <list>

using namespace std;

int main() {

   list<int> lst;
   for(int i = 1; i <= 20; ++i) {
      lst.push_front(i*10); 
   }

   list<int>::iterator it = next(lst.begin(), 5); // (or auto) access the iterator to the 5-th element of the list
   // NOTE: next(it, 5) is equal to it+5 only for random-access data structures, i.e. it won't work for list 
   lst.insert(it, 10); // place a new value at the iterator

   // printing the list
   for(const auto& e: lst) cout << e << " "; cout << endl;

   // removing the first 5 elements of the list
   for(int i = 0; i < 5; ++i) {
       lst.erase(lst.begin());
   }

   for(const auto& e: lst) cout << e << " "; cout << endl;
  
   return 0;
}
