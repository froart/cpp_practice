#include <iostream>
#include <fstream>

using namespace std;

string file_to_string(const char* filename) {

	ifstream input(filename,  ios::in);
	string text((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
  return text;
}

int main() {

  string file = file_to_string("./list.cpp");
	cout << file << endl;	

	return 0;
}
