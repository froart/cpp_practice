#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2pp/SDL2pp.hh>
#include <cstring>
#include <cassert>
#include <vector>
#include <algorithm>

using namespace std;

const int width = 800;
const int height = 600;

// Enabling extensions
vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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
   SDL2pp::SDL sdl(SDL_INIT_VIDEO);
   SDL2pp::Window window("My vulkan tutorial",
                         SDL_WINDOWPOS_UNDEFINED, 
                         SDL_WINDOWPOS_UNDEFINED,
                         width, height,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN); // Vulkan flag must be added in order to use SDL_Vulkan_CreateSurface()

#ifndef NDEBUG
   if(enableValidationLayers && !checkValidationLayerSupport()) // check if requested validation layers exist on the system
      throw runtime_error("Validation layers requested but not available");
#endif

   unsigned int count = 0;

#ifndef NDEBUG // retrieve the number of extensions required to ntegrate SDL with Vulkan
   assert(SDL_Vulkan_GetInstanceExtensions(window.Get(), &count, nullptr));
#else
   SDL_Vulkan_GetInstanceExtensions(window.Get(), &count, nullptr);
#endif

   vector<const char*> SDL_extensions_required(count);

#ifndef NDEBUG // retrieve the extensions required to integrate SDL with Vulkan
   assert(SDL_Vulkan_GetInstanceExtensions(window.Get(), &count, SDL_extensions_required.data()));
#else
   SDL_Vulkan_GetInstanceExtensions(window.Get(), &count, SDL_extensions_required.data());
#endif
   for(int i = 0; i < count; ++i) // fill the Vulkan extension list with the required SDL extensions
      instanceExtensions.push_back(SDL_extensions_required[i]);

   // Creating Vulkan instance
   vk::ApplicationInfo appInfo ( "Hello triangle", VK_MAKE_VERSION(1,0,0), "No Engine", 1, VK_MAKE_VERSION(1,0,0) );
#ifndef NDEBUG
   vk::InstanceCreateInfo instanceCreateInfo ( {}, &appInfo, validationLayers, instanceExtensions );
#else
   vk::InstanceCreateInfo instanceCreateInfo ( {}, &appInfo, {}, instanceExtensions );
#endif
   vk::Instance instance = vk::createInstance ( instanceCreateInfo ); ;

   // Binding SDL and Vulkan via Surface
   vk::SurfaceKHR surface;
   {
      VkSurfaceKHR _surface;
      assert(SDL_Vulkan_CreateSurface( window.Get(), static_cast<VkInstance>(instance), &_surface )); // bind Vulkan Surface to SDL Window
      surface = vk::SurfaceKHR( _surface ); // move assignment
   }

#ifndef NDEBUG
   // Create a customized DebugMessenger
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

   // Retrieve physical devices
   vk::PhysicalDevice physicalDevice = instance.enumeratePhysicalDevices().front(); // Choose the first found physical device

   // Query the physical device properties
   vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties(); // and retrieve its properties
   cout << "Chosen Physical Device Information:" << endl; // print chosen physical device's properties
   cout << "Device Name: " << physicalDeviceProperties.deviceName << endl;
   cout << "API Version: " << VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion) << "."
                           << VK_VERSION_MINOR(physicalDeviceProperties.apiVersion) << "."
                           << VK_VERSION_PATCH(physicalDeviceProperties.apiVersion) << endl;
   if(physicalDeviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) // Warn if a chosen device is not a GPU
     cerr << "Chosen physical device is not a descrete GPU!" << endl;

   // Query the physical device features
   vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures(); // Retrieve physical device Features
   if(!physicalDeviceFeatures.geometryShader) // Geometry Shader is a required feature
      throw runtime_error("Chosen physical device doesn't have a geometry shader!");

   // Query the physical device queue families
   vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
   auto propertyIt = find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(), // find eGraphics queue family
                             [](vk::QueueFamilyProperties const & qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; } );
   size_t graphicsQueueFamilyIndex = distance(queueFamilyProperties.begin(), propertyIt);
   size_t presentQueueFamilyIndex = physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( graphicsQueueFamilyIndex ), surface ) // check if the index has a surface support
                                    ? graphicsQueueFamilyIndex : queueFamilyProperties.size();
#ifndef NDEBUG
   assert( presentQueueFamilyIndex == graphicsQueueFamilyIndex );
#endif

   // Create a logical device
   float queuePriority = 0.0f;
   vk::DeviceQueueCreateInfo deviceQueueCreateInfo( vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &queuePriority );

   vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo, {}, deviceExtensions);
   vk::Device device = physicalDevice.createDevice( deviceCreateInfo );
   
   // Get supported formats
   vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR( surface );
#ifndef NDEBUG
   assert( !formats.empty() );
#endif

   vk::Format format = ( formats[0] == vk::Format::eUndefined ) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;
   vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR( surface );
   vk::Extent2D swapchainExtent;
   if( surfaceCapabilities.currentExtent.width == numeric_limits<uint32_t>::max() ) { // if the furface resolution is underfined
      //... set it to the sizes of the image
      swapchainExtent.width = clamp( static_cast<uint32_t>(width), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width );
      swapchainExtent.height = clamp( static_cast<uint32_t>(height), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height );
   } else {
      // if defined, the swapchain resolution must match it
      swapchainExtent = surfaceCapabilities.currentExtent;
   }

   // set to not apply any tranformation upon image
   vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;
   vk::SurfaceTransformFlagBitsKHR preTransform = ( surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity )
                                                  ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                                                  : surfaceCapabilities.currentTransform;
   
   // set to not blend with other windows
   vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
   
   // defining the swapchain create information 
   vk::SwapchainCreateInfoKHR swapchainCreateInfo( vk::SwapchainCreateFlagsKHR(),
                                                   surface,
                                                   // define the number of image in the swapchain
                                                   clamp( static_cast<uint32_t>(3), surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount ),
                                                   format,
                                                   vk::ColorSpaceKHR::eSrgbNonlinear,
                                                   swapchainExtent,
                                                   1,
                                                   vk::ImageUsageFlagBits::eColorAttachment,
                                                   vk::SharingMode::eExclusive,
                                                   {},
                                                   preTransform,
                                                   compositeAlpha,
                                                   swapchainPresentMode,
                                                   true,
                                                   nullptr );

   vk::SwapchainKHR swapchain = device.createSwapchainKHR ( swapchainCreateInfo );
                                                  
   // Main Rendering Loop
   bool quit = false;
   while(!quit) { 
      SDL_Event event;
      SDL_PollEvent(&event);
      if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) // if ESC is hit
         quit = true;
   }

   // cleanup 
   device.destroySwapchainKHR( swapchain );
   instance.destroySurfaceKHR( surface );
   device.destroy();
#ifndef NDEBUG
// instance.destroyDebugUtilsMessengerEXT( debugUtilsMessenger );
#endif
   instance.destroy();
   return EXIT_SUCCESS;

} catch(SDL2pp::Exception& e) {
   cerr << "Error in: " << e.GetSDLFunction() << endl;
   cerr << "   Reason: " << e.GetSDLError() << endl;
   return EXIT_FAILURE;

} catch(const exception& e) {
   cerr << e.what() << endl;
   return EXIT_FAILURE;

} 
