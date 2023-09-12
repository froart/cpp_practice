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
#include <array>
#include <algorithm>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <fstream>
#include "default_resources.hpp"

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

EShLanguage translateShaderStage(const vk::ShaderStageFlagBits shaderType) {
   switch( shaderType ) {
      case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
      case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
      default: cout << "Unknown stage shader"; return EShLangVertex;
   }
}

bool GLSLtoSPV( const vk::ShaderStageFlagBits shaderType, string const & glslShader, vector<unsigned int> & spvShader ) {
   
   EShLanguage stage = translateShaderStage( shaderType );

   const char* shaderStrings[1];
   shaderStrings[0] = glslShader.data();

   glslang::TShader shader( stage );
   shader.setStrings( shaderStrings, 1 );

   TBuiltInResource Resources = InitResources();      

   EShMessages messages = (EShMessages) ( EShMsgSpvRules | EShMsgVulkanRules );

   if( !shader.parse( &Resources, 100, false, messages ) ) {
      cerr << shader.getInfoLog() << endl << shader.getInfoDebugLog() << endl;
      return false;
   }

   glslang::TProgram program;
   program.addShader( &shader );

   if( !program.link( messages ) ) {
      cout << shader.getInfoLog() << endl << shader.getInfoDebugLog() << endl;
      fflush( stdout );
      return false;
   }

   glslang::GlslangToSpv( *program.getIntermediate( stage ), spvShader );
   return true; 
}

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

#ifndef NDEBUG
   cout << "Surface capabilities image count: min: " << surfaceCapabilities.minImageCount << ", max: " << surfaceCapabilities.maxImageCount << endl;
#endif

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
                                                   // FIXME for some reason surfaceCapabilities.maxImageCount == 0
                                                   //clamp( static_cast<uint32_t>(3), surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount ),
                                                   3u,
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
                                                  
   vector<vk::Image> swapchainImages = device.getSwapchainImagesKHR( swapchain );
   vector<vk::ImageView> imageViews;
   imageViews.reserve( swapchainImages.size() );
   vk::ImageViewCreateInfo imageViewCreateInfo( {}, {}, vk::ImageViewType::e2D, format, {}, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } );
   for ( auto image : swapchainImages ) {
      imageViewCreateInfo.image = image;
      imageViews.push_back( device.createImageView( imageViewCreateInfo ) );
   }

   // Compile the GLSL shaders
   glslang::InitializeProcess();
   // Creating vertex shader module
   vector<unsigned int> vertexShaderSPV; // actual SPIR-V bytecode
   // File to string
   ifstream vertexShaderFile("../shader.vert", ios::in);
   string vertexShaderString((istreambuf_iterator<char>(vertexShaderFile)), istreambuf_iterator<char>());
#ifndef NDEBUG
   assert(
#endif
   GLSLtoSPV(vk::ShaderStageFlagBits::eVertex, vertexShaderString, vertexShaderSPV )
#ifndef NDEBUG
   );
#else
   ;
#endif
   vk::ShaderModuleCreateInfo vertexShaderModuleCreateInfo( vk::ShaderModuleCreateFlags(), vertexShaderSPV);
   vk::ShaderModule vertexShaderModule = device.createShaderModule( vertexShaderModuleCreateInfo );

   // Creating fragment shader module
   vector<unsigned int> fragmentShaderSPV; // actual SPIR-V bytecode
   ifstream fragmentShaderFile("../shader.frag", ios::in);
   string fragmentShaderString((istreambuf_iterator<char>(fragmentShaderFile)), istreambuf_iterator<char>());
#ifndef NDEBUG
   assert(
#endif
   GLSLtoSPV( vk::ShaderStageFlagBits::eFragment, fragmentShaderString, fragmentShaderSPV )
#ifndef NDEBUG
   );
#else
   ;
#endif
   vk::ShaderModuleCreateInfo fragmentShaderModuleCreateInfo( vk::ShaderModuleCreateFlags(), fragmentShaderSPV);  
   vk::ShaderModule fragmentShaderModule = device.createShaderModule( fragmentShaderModuleCreateInfo );
   glslang::FinalizeProcess();

   //  Creation of Pipelline Shader Stages
   array<vk::PipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos = {
    vk::PipelineShaderStageCreateInfo( vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main"),
    vk::PipelineShaderStageCreateInfo( vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main")
  };

   // Dynamic states of the pipeline
   array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor};
   vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo( vk::PipelineDynamicStateCreateFlags(), dynamicStates );

   // Vertex data format setup
   vk::VertexInputBindingDescription vertexInputBindingDescription( 0, sizeof(float) * 7); // 3 for coordinate, 4 for color
   array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescriptions = {
      vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32B32A32Sfloat, 0 ),
      vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32A32Sfloat, 12 ),
  };
  vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo( vk::PipelineVertexInputStateCreateFlags(),
     vertexInputBindingDescription,
     vertexInputAttributeDescriptions
   );
   // specify what kind of geometry will be drawn from vertices
   vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo( vk::PipelineInputAssemblyStateCreateFlags(), 
          vk::PrimitiveTopology::eTriangleList );
   // setup the viewport
   vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f);
   vk::Rect2D scissor({0, 0}, swapchainExtent);
   vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo( vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor);
   // setting up the rasterizer
   vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo( vk::PipelineRasterizationStateCreateFlags(),
         false, // depthClampEnable / requires GPU enabling
         false, // rasterizerDiscardEnable / prevents vertices to get to the frame buffer 
         vk::PolygonMode::eFill,
                                                                                  vk::CullModeFlagBits::eBack,
                                                                                  vk::FrontFace::eClockwise,
                                                                                  false, // depthBiasEnable
                                                                                  0.0f,  // depthBiasConstantFactor
                                                                                  0.0f,  // depthBiasClamp
                                                                                  0.0f,  // depthBiasSlopeFactor
                                                                                  1.0f   // lineWidth
                                                                                );
   // multisampling setup (one of ways to preform anti-aliasing)
   vk::PipelineMultisampleStateCreateInfo pipelineMultisaampleStateCreateInfo( vk::PipelineMultisampleStateCreateFlags(),
                                                                               vk::SampleCountFlagBits::e1,
                                                                               false, // sampleShadingEnable
                                                                               1.0f, // minSampleShading
                                                                               nullptr, // pSampleMask
                                                                               false, // alphaToCoverageEnable
                                                                               false // alphaToOneEnable
    );
   vk::StencilOpState stencilOpState( vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways); // the rest is optional
   vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo( vk::PipelineDepthStencilStateCreateFlags(),
                                                                               true, // depthTestEnable
                                                                               true, // depthWriteEnable
                                                                               vk::CompareOp::eLessOrEqual, // depthCompareOp
                                                                               false, // depthBoundTestEnable
                                                                               false, // stencilTestEnable
                                                                               stencilOpState, // front
                                                                               stencilOpState // back
                                                                              );
   vk::ColorComponentFlags colorComponentFlags( vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA );
   vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState( false, // blendEnable
                                                                            vk::BlendFactor::eZero, // srcColorBlendFactor
                                                                            vk::BlendFactor::eZero, // sdtColorBlendFactro
                                                                            vk::BlendOp::eAdd, // colorBlendOp
                                                                            vk::BlendFactor::eZero, // srcAlphaBlendFactor
                                                                            vk::BlendFactor::eZero, // dstAlphaBlendFactor
                                                                            vk::BlendOp::eAdd, // AlphaBlendOp
                                                                            colorComponentFlags
                                                                          );
    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo( vk::PipelineColorBlendStateCreateFlags(),
                                                                            false, // logicOpEnable
                                                                            vk::LogicOp::eNoOp, // logicOp
                                                                            pipelineColorBlendAttachmentState, // attachments
                                                                            { {1.0f, 1.0f, 1.0f, 1.0f } } // blendContrasts
                                                                           );

   // Setting up the pipeline layout 
   // used to specify global variables which will be used by the pipeline, e.g. transformation matrices
   vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo( 
       vk::PipelineLayoutCreateFlags(), 
       0, // setLayoutCount
       nullptr,  // pSetLayouts
       0, // pushConstantRangeCount 
       nullptr ); // pPushConstantRanges
   vk::PipelineLayout pipelineLayout = device.createPipelineLayout( vk::PipelineLayoutCreateInfo( pipelineLayoutCreateInfo ) );

   // Render Pass 
   // "...We need to specify how many color and depth buffers there will be, 
   // how many samples to use for each of them and how their contents should be handled throughout the rendering operations. 
   // All of this information is wrapped in a render pass object..."
   vector<vk::AttachmentDescription> attachmentDescriptions;
   attachmentDescriptions.emplace_back( 
      vk::AttachmentDescriptionFlags(), 
      format, // we take from a swapchain color format
      vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp::eStore,
      vk::AttachmentLoadOp::eDontCare, // stencil loagOp
      vk::AttachmentStoreOp::eDontCare, // stencil storeOp
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::ePresentSrcKHR
   );
   vk::AttachmentReference colorAttachment( 0, vk::ImageLayout::eColorAttachmentOptimal );
   // vk::AttachmentReference depthAttachment( 1, vk::ImageLayout::eDepthStencilAttachmentOptimal );
   vk::SubpassDescription subpassDescription( vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, {}, colorAttachment, {}, nullptr);
   vk::RenderPass renderPass = device.createRenderPass( vk::RenderPassCreateInfo( vk::RenderPassCreateFlags(), attachmentDescriptions, subpassDescription ) );
   vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo( vk::PipelineCreateFlags(),
      pipelineShaderStageCreateInfos,
      &pipelineVertexInputStateCreateInfo,
      &pipelineInputAssemblyStateCreateInfo,
      nullptr, // Tessalation Stage
      &pipelineViewportStateCreateInfo,
      &pipelineRasterizationStateCreateInfo,
      &pipelineMultisaampleStateCreateInfo,
      &pipelineDepthStencilStateCreateInfo,
      &pipelineColorBlendStateCreateInfo,
      &pipelineDynamicStateCreateInfo,
      pipelineLayout,
      renderPass
   );
   // Creating graphics pipeline
   auto [result, pipeline] = device.createGraphicsPipeline( nullptr, graphicsPipelineCreateInfo );
   if(result != vk::Result::eSuccess)
     throw runtime_error("Couldn't create graphics pipeline!");
   // Creating Frame buffers
   vector<vk::Framebuffer> swapchainFramebuffers;
   for(auto const & imageView : imageViews) {
     vk::ImageView attachment = { imageView };
     vk::FramebufferCreateInfo framebufferCreateInfo( vk::FramebufferCreateFlags(), renderPass, attachment, swapchainExtent.width, swapchainExtent.height, 1 );
     swapchainFramebuffers.push_back( device.createFramebuffer( framebufferCreateInfo) );
   }
   // Creating Command Pool
   vk::CommandPool commandPool = device.createCommandPool( vk::CommandPoolCreateInfo( vk::CommandPoolCreateFlags(), graphicsQueueFamilyIndex ) );
   vk::CommandBuffer commandBuffer = device.allocateCommandBuffer( vk::CommandBufferAllocateInfo( commandPool, vk::CommandBufferLevel::ePrimary, 1 ) ).front();
   // Main Rendering Loop
   bool quit = false;
   while(!quit) { 
      SDL_Event event;
      SDL_PollEvent(&event);
      if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)          // if ESC is hit
         quit = true;
   }

   // cleanup 
  device.freeCommandBuffers( commandPool, commandBuffer );
  device.destroyCommandPool( commandPool );
  for(auto const & swapchainFramebuffer : swapchainFramebuffers)
    device.destroyFramebuffer( swapchainFramebuffer );
  device.destroyPipeline( pipeline );
  device.destroyRenderPass ( renderPass );
  device.destroyPipelineLayout( pipelineLayout );
  device.destroyShaderModule( fragmentShaderModule );
  device.destroyShaderModule( vertexShaderModule );
   for( auto & imageView : imageViews )
      device.destroyImageView( imageView );
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
