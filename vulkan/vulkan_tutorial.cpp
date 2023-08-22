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
#ifndef NDEBUG
const vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

#ifndef NDEBUG
//PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
//VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT( VkInstance                                 instance,
//                                                             const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
//                                                             const VkAllocationCallbacks *              pAllocator,
//                                                             VkDebugUtilsMessengerEXT *                 pMessenger ) {
// return pfnVkCreateDebugUtilsMessengerEXT( instance, pCreateInfo, pAllocator, pMessenger );
//}
//VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
//                                              VkDebugUtilsMessengerCallbackDataEXT const * pCallbackData,
//                                              void* pUserData) {
//      cerr << "Validation layer: " << pCallbackData->pMessage << endl;
//      return VK_FALSE;
//}
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
#endif

int main(int argc, char** argv) try {

   // Creating SDL window
   SDL2pp::Window window("My vulkan tutorial",
                         SDL_WINDOWPOS_UNDEFINED, 
                         SDL_WINDOWPOS_UNDEFINED,
                         width, height,
                         SDL_WINDOW_SHOWN);
   SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

   // Initializing Vulkan
#ifndef NDEBUG
   if(enableValidationLayers && !checkValidationLayerSupport()) // check if requested validation layers exist on the system
      throw runtime_error("Validation layers requested but not available");
#endif

   vk::ApplicationInfo appInfo ( "Hello triangle", VK_MAKE_VERSION(1,0,0), "No Engine", 1, VK_MAKE_VERSION(1,0,0) );
   vk::InstanceCreateInfo instanceCreateInfo ( {}, &appInfo );

#ifndef NDEBUG
   instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
   instanceCreateInfo.ppEnabledLayerNames = validationLayers.data(); // add validation layers
#else
// instanceCreateInfo.enabledLayerCount = 0;
// instanceCreateInfo.pNext = nullptr;
#endif

   vk::Instance instance = vk::createInstance ( instanceCreateInfo ); ;

#ifndef NDEBUG
/* FIXME the function vkCreateDebugUtilsMessengerEXT throws a segfault
   vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
   vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                       vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                       vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
   vk::DebugUtilsMessengerEXT debugUtilsMessenger = instance.createDebugUtilsMessengerEXT (vk::DebugUtilsMessengerCreateInfoEXT ( {}, severityFlags, messageTypeFlags, &debugCallback) ); 
*/
#endif

   vk::PhysicalDevice physicalDevice = instance.enumeratePhysicalDevices().front();
   vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();

   cout << "Chosen Physical Device Information:" << endl;
   cout << "Device Name: " << physicalDeviceProperties.deviceName << endl;
   cout << "API Version: " << VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion) << "."
        << VK_VERSION_MINOR(physicalDeviceProperties.apiVersion) << "."
        << VK_VERSION_PATCH(physicalDeviceProperties.apiVersion) << endl;

   vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures(); 
   
   if(physicalDeviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
     cerr << "Chosen physical device is not a descrete GPU!" << endl;

   if(!physicalDeviceFeatures.geometryShader)
     cerr << "Chosen physical device doesn't have a geometry shader!" << endl;

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
