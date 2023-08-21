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
    
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                              vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                              const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                              void* pUserData) {
      cerr << "Validation layer: " << pCallbackData->pMessage << endl;
      return VK_FALSE;
}

int main(int argc, char** argv) try {

   // Creating SDL window
   SDL2pp::Window window("My vulkan tutorial",
                         SDL_WINDOWPOS_UNDEFINED, 
                         SDL_WINDOWPOS_UNDEFINED,
                         width, height,
                         SDL_WINDOW_SHOWN);
   SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

   // Initializing Vulkan
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
      // SETUP DEBUGGER
      vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
      vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
      vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo { .messageSeverity = severityFlags, 
                                                             .messageType = messageTypeFlags, 
                                                             .pfnUserCallback = nullptr /*debugCallback*/};
                                                             // FIXME debugCallback should be of a valid type
      instanceCreateInfo.pNext = &debugCreateInfo; // Pass debugger info to the instance info
   } else {
      instanceCreateInfo.enabledLayerCount = 0;
      instanceCreateInfo.pNext = nullptr;
   }
   vk::Instance instance;
   if(vk::createInstance(&instanceCreateInfo, nullptr, &instance) != vk::Result::eSuccess) // creating instance
      throw std::runtime_error("failed to create instance!");

   // Main loop
   bool quit = false;
   while(!quit) {
      SDL_Event event;
      SDL_PollEvent(&event);
      if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
         quit = true;
      renderer.Present();
   }

   return EXIT_SUCCESS;

} catch(const exception& e) {

   cerr << e.what() << endl;

   return EXIT_FAILURE;
}
