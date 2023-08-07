#include <iostream>
#include <vector>
#include <fstream>
#include <array>
#include <memory>
#include <algorithm>

using namespace std;

int main() {

   // array initialized at compile time
   // no memory management is needed
   // you cannot resize it
   int mat[3][3] = {
                   {1, 3, 4},
                   {3, 4, 6},
                   {4, 2, 1}
                   };
   // array initialized at runtime
   // no memory management is needed
   // can be resized
   vector<vector<int>> mat2 = {
                   {1, 3, 4},
                   {3, 4, 6},
                   {4, 2, 1}
                   };

   // array initialized at runtime
   // no memory management is needed
   // can be resized
   vector<vector<int>> mat3(3, vector<int>(3));

   mat3 = {{1,2,3}, {4,5,6}, {7,8,9}};

   array<array<int, 10>, 10> arr; // matrix 10x10
   arr[0][1] = 1;
   arr[1][2] = 2;
   arr[9][9] = 100;

   // initialize matrix
   for(int j = 0; j < arr.size(); ++j)
      for(int i = 0; i < arr[0].size(); ++i)
         arr[i][j] = i + j * arr[0].size();

   // print matrix
   for(auto& sub_arr: arr)
      for(auto& e: sub_arr)
         cout << e << ((&e == &sub_arr.back()) ? '\n' : ' ');

   return 0;
}
