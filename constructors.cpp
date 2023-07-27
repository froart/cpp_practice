#include <iostream>
#include <exception>
using namespace std;

template <typename T>
class Darray {
   private:
      T* array;
      int size;
   public:
      Darray<T>() : array(nullptr), size(0) { cout << "default constructor is called" << endl;} // default constructor
      Darray<T>(T* array0, int size0) : size(size0) { // parametrized constructor
          array = new T[size]; 
          for(int i = 0; i < size; ++i)
              array[i] = array0[i];
          cout << "parametrized constructor is called" << endl;
      }
      Darray<T>(const Darray& darray0): size(darray0.size) { // copy constructor
          array = new T[size]; 
          for(int i = 0; i < size; ++i)
              array[i] = darray0.array[i];
          cout << "copy constructor is called" << endl;
      }
      Darray<T>(Darray<T>&& darray0) : array(darray0.array), size(darray0.size) {
          darray0.array = nullptr;
          darray0.size = 0;
          cout << "move constructor is called" << endl;
      }
      ~Darray<T>() { 
          size = 0; 
          delete[] array; 
          cout << "destructor is called" << endl;
      }
      T& operator[](int index) { // the return type is reference, since the element access is also used for assigning value
         return array[index];
      }
      T* begin() { return array; }
      T* end() { return &array[size-1]; }
      T& at(int index) { 
         try {
             if(index < 0 or index >= size) throw out_of_range("The index is out of range"); 
         } catch(const exception& e) {
             cout << e.what() << endl;
         }
         return array[index];
      }
};

int main() {

    int arr[] = {1,2,3,4,5,6,7,8,9,10};
    Darray<int> darr1;
    Darray<int> darr2(arr, sizeof(arr)/sizeof(arr[0]));
    Darray<int> darr3(darr2);
    Darray<int> darr4(move(darr3));
    for(const auto& e : darr4)
        cout << e << " ";
    cout << endl;
    cout << darr4.at(22) << endl;
    return 0;
}
