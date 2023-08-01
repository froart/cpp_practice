#include <iostream>
#include <string>
#include <map>
#include <chrono>

using namespace std;

int main () {

	map<int, string, less<>> solar_system;
  // third template argument is a constexpr sorting rule
  // in this case sort in ascending order

  // element insertion
	solar_system[0] = "Sun" ; // this inserts a new element
	solar_system[1] = "Mercury";
	solar_system[2] = "Venus";
	solar_system[3] = "Earth";
	solar_system[4] = "Mars";
	solar_system[5] = "Jupiter";
	solar_system[6] = "Saturn";
  // another way to insert an element (more complicated)
  // m0.insert(make_pair("earth", 3));
  // OR
  // multiple insertion
  solar_system.insert({{7, "Uranus"}, {8, "Neptune"}});
  // insertion with return values
  const auto [iter, success] = solar_system.insert({10, "Pluto"});
  cout << "Planet " << iter->second << " inserted " << (success ? "successfully" : "unsuccessfully") <<  endl;

  // Changin the key (with reallocation)
  string value = solar_system[10]; // save the value
  solar_system.erase(10);
  solar_system.insert({9, value});
  // OR a hack way (without reallocation)
  // auto pluto_node = solar_system.extract(10); 
  // pluto_node.key() = 9;
  // solar_system.insert(move(pluto_node));

  // print map
	for(const auto& [k, v] : solar_system) cout << k << ": " << v << endl;

  // erase an element
  int num = solar_system.erase(9); // returns a number of elements erased
  // since keys are unique this operation will always return either 0 or 1
  cout << "number of planets erased: " << num << endl;

  // print map
	for(const auto& [k, v] : solar_system)	cout << k << ": " << v << endl;

  // initialized map
  map<string, int> ages = {{"Artemiy", 28}, {"Krzysztof", 70}, {"Anton", 32}};

  // another way to print map (more complicated)
  map<string, int>::iterator it = ages.begin();
  while(it != ages.end()) {
      cout << "name: " << it->first << ", age: " << it->second << endl;
      ++it;
  }
  
  // Find the iterator to the key-value pair by key
  // NOTE: the init-statement in if is a feature of C++17
  if(auto search = ages.find("Anton"); search != ages.end()) { 
     const auto [k, v] = make_pair(search->first, search->second); // for an often use further
     cout << "Found " << k << ": " << v << endl;
  }
 
  ages.insert_or_assign("Artemiy", 29);
  for(auto [k, v]: ages) cout << k << ": " << v << endl;

	return 0;
}
