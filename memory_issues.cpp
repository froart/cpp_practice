#include <iostream>
#include <vector>

using namespace std;

int main() {

    // Memory leak scenario #1
    int* ptr = new int[12]; // dynamic memory allocation, which then will require deallocation
    int* ptr2 = new int[1233];
    ptr = ptr2; // memory leak! the array with 12 elements is no longer accessible and cannot be deallocated 
    delete[] ptr; // this will dealocate the array with 1233 elements

   // Segmentation fault scenario #1
   vector<int> ages; // size 0
   const int& age = ages.back(); // reference to last element of the vector
   ages.push_back(23); // assignment, but will cause reallocation
   // age now refence to an illegal memory address
   cout << age << endl; // this will cause segfoult

   return 0;
}
