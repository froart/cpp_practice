#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;


int main() {

   unordered_map<int, string> u_map = {{1, "Peter"}, {2, "John"}, {3, "Chris"}};
   u_map[4] = "George"; 
   u_map[0] = "Suleyman";

   cout << endl << "u_map" << endl;
   for(const auto& [k, v]: u_map)
      cout << k << ":" << v << endl;

   auto result = u_map.insert_or_assign(5, "Gregory");
   auto iter = result.first;
   auto success = result.second;

   cout << endl << iter->first << ":" << iter->second << " " << ((success == true) ? "is added" : "is assigned") << endl;;

   result = u_map.insert_or_assign(0, "Tray");
   iter = result.first;
   success = result.second;

   cout << endl << iter->first << ":" << iter->second << " " << ((success == true) ? "is added" : "is assigned") << endl;;

   cout << endl << "u_map" << endl;
   for(const auto& [k, v]: u_map)
      cout << k << ":" << v << endl;

   return 0;
}
