#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

using namespace std;

const int width = 640;
const int height = 400;

void updateFrameBuffer() {
   return;
}

int main() {
   SDL2pp::Window* window;
   SDL2pp::Renderer* renderer;
   try {
      window = new SDL2pp::Window("My vulkan tutorial",
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              width, height,
                              SDL_WINDOW_SHOWN);
      renderer = new SDL2pp::Renderer(*window, -1, SDL_RENDERER_ACCELERATED);
   } catch(SDL2pp::Exception& e) {
      std::cerr << "Error in: " << e.GetSDLFunction() << std::endl;
      std::cerr << "  Reason: " << e.GetSDLError() << std::endl;
   }

   bool quit = false;
   while(!quit) {
      SDL_Event event;
      SDL_PollEvent(&event);
      if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
         quit = true;
      updateFrameBuffer();
      renderer->Present();
   }
   delete renderer;
   delete window;
   return 0;
}
