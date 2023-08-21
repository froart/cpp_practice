// remove all union and structure constructors to use C++20 designated initializer feature
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
    
      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
         vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
         vk::DebugUtilsMessageTypeFlagsEXT messageType,
         const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
         void* pUserData) {
            cerr << "Validation layer: " << pCallbackData->pMessage << endl;
            return VK_FALSE;
      }
    
      void initWindow() {
         window_ = new SDL2pp::Window("My vulkan tutorial",
                                      SDL_WINDOWPOS_UNDEFINED, 
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width, height,
                                      SDL_WINDOW_SHOWN);
         renderer_ = new SDL2pp::Renderer(*window_, -1, SDL_RENDERER_ACCELERATED);
      }

      void initVulkan() {
         // CREATE INSTANCE
         if(enableValidationLayers && !checkValidationLayerSupport()) // check if requested validation layers exist on the system
            throw runtime_error("Validation layers requested but not available");
         vk::ApplicationInfo appInfo{ .pApplicationName = "Hello triangle", // specifying application information
                                      .applicationVersion = VK_MAKE_VERSION(1,0,0),
                                      .pEngineName = "No Engine",
                                      .engineVersion = VK_MAKE_VERSION(1,0,0),
                                      .apiVersion = VK_API_VERSION_1_0 };
         vk::InstanceCreateInfo instanceCreateInfo { .pApplicationInfo = &appInfo }; // specifying instance information
         if(enableValidationLayers) {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data(); // add validation layers
         } else instanceCreateInfo.enabledLayerCount = 0;
         if(vk::createInstance(&instanceCreateInfo, nullptr, &instance_) != vk::Result::eSuccess) // creating instance
            throw std::runtime_error("failed to create instance!");
         // SETUP DEBUGGER
         if(enableValidationLayers) {
            vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
            vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                                  vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                                  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
            vk::DebugUtilsMessengerEXT debugUtilsMessenger = instance_.createDebugUtilsMessengerEXT(
                                                                           vk::DebugUtilsMessengerCreateInfoEXT {
                                                                              .messageSeverity = severityFlags, 
                                                                              .messageType = messageTypeFlags, 
                                                                              .pfnUserCallback = nullptr /*debugCallback*/});
                                                                              // FIXME debugCallback should be of a valid type
         }
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
