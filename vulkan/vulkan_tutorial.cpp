#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <cstring>

using namespace std;

const int width = 800;
const int height = 600;

// Enabling validation layers
const vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

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
      vk::Instance instance_;

      bool checkValidationLayerSupport() {
         unsigned int layerCount;
         vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
         vector<vk::LayerProperties> availableLayers(layerCount);
         vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
         for(const char* layerName : validationLayers) {
            bool layerFound = false;
            for(const auto& layerProperties : availableLayers) {
               if(strcmp(layerName, layerProperties.layerName) == 0) {
                  layerFound = true;
                  break;
               }
            }
            if(!layerFound) return false;
         }
         return true;
      }

      void initWindow() {
         window_ = new SDL2pp::Window("My vulkan tutorial",
                                      SDL_WINDOWPOS_UNDEFINED, 
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width, height,
                                      SDL_WINDOW_SHOWN);
         renderer_ = new SDL2pp::Renderer(*window_, -1, SDL_RENDERER_ACCELERATED);
      }

      void createInstance() {
         if(enableValidationLayers && !checkValidationLayerSupport())
            throw runtime_error("Validation layers requested but not available");
         vk::ApplicationInfo appInfo{ .pApplicationName = "Hello triangle",
                                      .applicationVersion = VK_MAKE_VERSION(1,0,0),
                                      .pEngineName = "No Engine",
                                      .engineVersion = VK_MAKE_VERSION(1,0,0),
                                      .apiVersion = VK_API_VERSION_1_0 };
         vk::InstanceCreateInfo createInfo{ .pApplicationInfo = &appInfo };
         if(enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
         } else createInfo.enabledLayerCount = 0;
         if(vk::createInstance(&createInfo, nullptr, &instance_) != vk::Result::eSuccess)
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
