#include <iostream>
#include <vector>
#include <fstream>
#include <array>
#include <memory>
#include <algorithm>

using namespace std;

template<typename T>
class Matrix {
   private:
       int rowNum_;
       int colNum_;
       vector<vector<T>> data_;
   public:
       // Constructors
       Matrix() { // default constructor
          this->rowNum_ = 0;
          this->colNum_ = 0;
       }; 
       Matrix(int rowNum, int colNum) { // parametrized constructor #1
          this->resize(rowNum, colNum);
          for(auto& row: this->data_) // assign 0 to each element of the matrix
             for(auto& element: row)
                element = (T)0; // statically cast to type T
       };
       Matrix(initializer_list<initializer_list<T>> lst) { // parametrized constructor #2
          // find the longest nested initializer_list
          int size = 0;
          for(const auto& e: lst)
             if(e.size() > size) size = e.size();
          // resize matrix accoring to initializer_list
          this->resize(lst.size(), size);
          // map initializer_list values to out matrix
          auto lst_row_it = lst.begin();
          for(int j = 0; j < this->rowNum_; ++j) {
             for(int i = 0; i < this->colNum_; ++i) {
                this->data_[j][i] = (i > lst_row_it->size()) ? 0 : *(lst_row_it->begin() + i); // fill with zeroes for the incomplete initializer_lists
             }
             ++lst_row_it;
          }
       };
       Matrix<T>& operator=(initializer_list<initializer_list<T>> lst) {
          Matrix<T> result = lst;
          *this = result; // copy
          return *this;
       }
       Matrix(const Matrix<T>& other) { // copy constructor
          this->resize(other.rowNum_, other.colNum_);
          this->data_ = other.data_;
       };
       Matrix<T>& operator=(const Matrix<T>& other) { // copy assign constructor
          this->resize(other.rowNum_, other.colNum_);
          this->data_ = other.data_;
          return *this;
       }
       // usefull member functions
       void resize(int rowNum, int colNum) {
          this->data_.resize(rowNum); // resize the number of rows of the matrix
          for(auto& row: this->data_) // resize each row
             row.resize(colNum);
          this->rowNum_ = rowNum;
          this->colNum_ = colNum;
       }
       void print() {
          for(int i = 0; i < this->colNum_; ++i)
             cout << "--";
          cout << endl;
          for(const auto& row: this->data_)
             for(const auto& element: row)
                cout << element << ((&element == &row.back()) ? '\n' : ' ');
          for(int i = 0; i < this->colNum_; ++i)
             cout << "--";
          cout << endl;
       };
       // operator overloads
       Matrix<T> operator*(const Matrix<T>& other) {
          if(this->colNum_ != other.rowNum_) // check matrices compatibility
             throw invalid_argument("Matrices can't be multiplied due to their size incompatibility!");
          Matrix<T> result = Matrix<T>(this->rowNum_, other.colNum_);
          for(int j = 0; j < this->rowNum_; ++j)
             for(int i = 0; i < other.colNum_; ++i)
                for(int k = 0; k < this->colNum_; ++k)
                   result.data_[j][i] += this->data_[j][k] * other.data_[k][i];
          return result;
       }; 
       Matrix<T> operator+(const Matrix<T> other) {
          if(this->rowNum_ != other.rowNum_ || this->colNum_ != other.colNum_) // check matrices compatibility
             throw invalid_argument("Matrices can't be summed up due to their size incompatibility!");
          Matrix<T> result(this->rowNum_, this->colNum_);
          for(int j = 0; j < this->rowNum_; ++j)
             for(int i = 0; i < this->colNum_; ++i)
                result.data_[j][i] = this->data_[j][i] + other.data_[j][i];
          return result;
          
       }
};

int main() {

   Matrix<int> mat1 = {{1,1}, 
                  {1,1,1},
                  {1}};
   Matrix<int> mat2 = {{1,1,1},
                  {1,1,1},
                  {1,1,1}};

   cout << "mat1:" << endl;
   mat1.print();
   cout << "mat2:" << endl;
   mat2.print();

   Matrix<int> mat3; // default constructor
   mat3 = {{1,2,3}, {4,5,6}, {7,8,9}}; // overload of the operator=(initializer_list<initializer_list<int>>)

   Matrix<int> mat4 = mat1 * mat2;
   cout << "Multiplication of mat1 and mat2: " << endl;
   mat4.print();

   Matrix<int> identity = {{1,0,0},
                           {0,1,0},
                           {0,0,1}}; 
   Matrix<int> mat5 = identity * identity;
   cout << "Multiplication of two identity matrices: " << endl;
   mat5.print(); // must still be identity matrix

   cout << "Summation of mat2 with itself:" << endl;
   Matrix<int> mat6 = mat2 + mat2;
   mat6.print();

   return 0;
}
