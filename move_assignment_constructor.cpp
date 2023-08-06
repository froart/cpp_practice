#include <iostream>
#include <vector>
#include <utility>

using namespace std;

class Image {

   private:
      class Pixel {
         public:
            Pixel() { // default constructor (should be defined to use with vector)
               this->colIndex_ = 0; 
               this->rowIndex_ = 0; 
            }
            Pixel(int colIndex, int rowIndex) { // parametrized constructor
               this->colIndex_ = colIndex; 
               this->rowIndex_ = rowIndex; 
            };
            Pixel(Pixel&& other) noexcept { // move constructor (should be defined to use with vector)
               this->colIndex_ = exchange(other.colIndex_, 0); // a hard way, but just to remember this command
               this->rowIndex_ = exchange(other.rowIndex_, 0); // it kinda gives a better clearance that we use the move operation here
            };
            Pixel& operator=(Pixel&& other) noexcept { // move assignment constructor
               if(this != &other) {
                  this->colIndex_ = exchange(other.colIndex_, 0); // a hard way, but just to remember this command
                  this->rowIndex_ = exchange(other.rowIndex_, 0); // it kinda gives a better clearance that we use the move operation here
               }
               return *this;
            };
            Pixel& operator=(Pixel& other) = delete; // explicitely delete copy assignment constructor (it is not necessary, added for understanding)
            // from now on any assignment will call move assignment constructor and never copy assignment one, so that there will be no ambiguities
            Pixel& operator=(initializer_list<unsigned char> color) { // color assignment operator
               if(color.size() != 4) throw invalid_argument("The size of initializer list must be exactly 4 elements");
               auto it = color.begin();
               this->color_.r = (*it);
               this->color_.g = (*(it+1));
               this->color_.b = (*(it+2));
               this->color_.a = (*(it+3));
               return *this;
            };

         private:
            struct Color {
               unsigned char r;
               unsigned char g;
               unsigned char b;
               unsigned char a;
            } color_;
            int colIndex_;
            int rowIndex_;
      };
      vector<Pixel> pixels_;
      int width_;
      int height_;
   public:
      Image(int width, int height) : width_(width), height_(height) {
         this->pixels_.resize(width * height);
         for(int j = 0; j < height; ++j) // contruct pixels
            for(int i = 0; i < width; ++i) {
               (*this)(i, j) = Pixel(i, j); // constructing pixel (it is gonna use the move assigment contructor here
               (*this)(i, j) = {0, 0, 0, 255}; // assigning a color to it
            }
      };
      Pixel& operator()(int colIndex, int rowIndex) { // pixel access
         return this->pixels_[colIndex + rowIndex * this->width_];
      };
};
int main() {

    Image image(800, 600);

    image(20, 30) = {23, 200, 34, 255}; // assign color to a pixel of an image

    return 0;
}
