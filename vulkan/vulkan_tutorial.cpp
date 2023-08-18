#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

using namespace std;

const int width = 640;
const int height = 400;

class TriangleApplication {
   public:
      void run() {
         initWindow();
         initVulkan();
         mainLoop();
         cleanUp();
      }

   private:
      SDL2pp::Window* window_;
      SDL2pp::Renderer* renderer_;
      VkInstance instance_;

      void initWindow() {
         window_ = new SDL2pp::Window("My vulkan tutorial",
                                      SDL_WINDOWPOS_UNDEFINED, 
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width, height,
                                      SDL_WINDOW_SHOWN);
         renderer_ = new SDL2pp::Renderer(*window_, -1, SDL_RENDERER_ACCELERATED);
      }

      void createInstance() {
         VkApplicationInfo appInfo{};
         appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
         appInfo.pApplicationName = "Hello triangle";
         appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
         appInfo.pEngineName = "No Engine";
         appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
         appInfo.apiVersion = VK_API_VERSION_1_0;
         VkInstanceCreateInfo createInfo{};
         createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
         createInfo.pApplicationInfo = &appInfo;
         createInfo.enabledLayerCount = 0;
         if(vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
            throw std::runtime_error("failed to create instance!");
      }

      void initVulkan() {
         createInstance();
      }

      void mainLoop() {
         bool quit = false;
         while(!quit) {
            SDL_Event event;
            SDL_PollEvent(&event);
            if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
               quit = true;
            //updateFrameBuffer();
            renderer_->Present();
         }
      }

      void cleanUp() {
         VkDestroyInstance(instance_, nullptr);
         delete renderer_;
         delete window_;
      }
};

int main() try {
   TriangleApplication app;
// glm::mat4 matrix;
// glm::vec4 vec;
// auto test = matrix * vec;
   app.run();
   return EXIT_SUCCESS;
} catch(const exception& e) {
   cerr << e.what() << endl;
   return EXIT_FAILURE;
}
