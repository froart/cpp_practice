#include <vulkan/vulkan.hpp>
// #include <vulkan/vulkan_raii.hpp>
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
#include  <limits>

using namespace std;

const int width  = 800;
const int height = 600;
const char* appName = "Vulkan Tutorial";
const uint32_t framesInFlight = 2;

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;
};

const vector<Vertex> vertices = {
  { {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
  { {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
  { { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
};

// TODO: DebugUtilsMessenger

// Enabling extensions
vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
vector<const char*> deviceExtensions   = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

// Enabling validation layers
#ifndef NDEBUG
const vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

#ifndef NDEBUG
// Check if specifies validationLayers are actually supported by the system
bool checkValidationLayerSupport() 
{
   unsigned int layerCount;
   vk::enumerateInstanceLayerProperties( &layerCount, nullptr );
   vector<vk::LayerProperties> availableLayers( layerCount );
   vk::enumerateInstanceLayerProperties( &layerCount, availableLayers.data() );
   for( const char* layerName : validationLayers ) 
   {
      bool layerFound = false;
      for( const auto& layerProperties : availableLayers )
      {
         if( strcmp( layerName, layerProperties.layerName ) == 0)
         {
            layerFound = true;
            break;
         }
      }
      if( !layerFound ) return false;
   }
   return true;
}
#endif

EShLanguage translateShaderStage( const vk::ShaderStageFlagBits shaderType )
{
   switch( shaderType ) 
   {
      case vk::ShaderStageFlagBits::eVertex:   return EShLangVertex;
      case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
      default: cout << "Unknown stage shader"; return EShLangVertex;
   }
}

string fileToString( string const & filename )
{
  ifstream file(filename, ios::in);
  return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

bool GLSLtoSPV( const vk::ShaderStageFlagBits shaderType, string const & glslShader, vector<unsigned int> & spvShader )
{
   
   EShLanguage stage = translateShaderStage( shaderType );

   const char* shaderStrings[1];
   shaderStrings[0] = glslShader.data();

   glslang::TShader shader( stage );
   shader.setStrings( shaderStrings, 1 );

   TBuiltInResource Resources = InitResources();      

   EShMessages messages = ( EShMessages ) ( EShMsgSpvRules | EShMsgVulkanRules );

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

int main( int /*argc*/, char** /*argv*/ ) try 
{

  // Creating SDL window
  SDL2pp::SDL sdl( SDL_INIT_VIDEO );
  SDL2pp::Window window( appName,
                         SDL_WINDOWPOS_UNDEFINED, 
                         SDL_WINDOWPOS_UNDEFINED,
                         width, height,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN ); // Vulkan flag must be added in order to use SDL_Vulkan_CreateSurface()

#ifndef NDEBUG
  if( enableValidationLayers && !checkValidationLayerSupport() ) // check if requested validation layers exist on the system
    throw runtime_error( "Validation layers requested, but not available!" );
#endif

  unsigned int count = 0;

#ifndef NDEBUG // retrieve the number of extensions required to ntegrate SDL with Vulkan
  assert( SDL_Vulkan_GetInstanceExtensions( window.Get(), &count, nullptr) );
#else
  SDL_Vulkan_GetInstanceExtensions( window.Get(), &count, nullptr );
#endif

  vector<const char*> SDL_extensions_required( count );

#ifndef NDEBUG // retrieve the extensions required to integrate SDL with Vulkan
  assert( SDL_Vulkan_GetInstanceExtensions( window.Get(), &count, SDL_extensions_required.data() ) );
#else
  SDL_Vulkan_GetInstanceExtensions( window.Get(), &count, SDL_extensions_required.data() );
#endif

  for( int i = 0; i < count; ++i ) // fill the Vulkan extension list with the required SDL extensions
    instanceExtensions.push_back( SDL_extensions_required[i] );

  vk::ApplicationInfo appInfo ( appName, VK_MAKE_VERSION(1,0,0), "No Engine", 1, VK_MAKE_VERSION(1,0,0) ); // Creating Vulkan instance

#ifndef NDEBUG
  vk::InstanceCreateInfo instanceCreateInfo ( {}, &appInfo, validationLayers, instanceExtensions );
#else
  vk::InstanceCreateInfo instanceCreateInfo ( {}, &appInfo, {},               instanceExtensions );
#endif

  vk::Instance instance = vk::createInstance ( instanceCreateInfo ); ;

  vk::SurfaceKHR surface; // Binding SDL and Vulkan via Surface
  {
    VkSurfaceKHR _surface;
    assert( SDL_Vulkan_CreateSurface( window.Get(), static_cast<VkInstance>( instance ), &_surface ) ); // bind Vulkan Surface to SDL Window
    surface = vk::SurfaceKHR( _surface ); // move assignment
  }

  vk::PhysicalDevice physicalDevice = instance.enumeratePhysicalDevices().front(); // Choose the first found physical device

  vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
  cout << "Chosen Physical Device Information:" << endl; // print chosen physical device's properties
  cout << "Device Name: " << physicalDeviceProperties.deviceName << endl;
  cout << "API Version: " << VK_VERSION_MAJOR( physicalDeviceProperties.apiVersion ) << "."
                          << VK_VERSION_MINOR( physicalDeviceProperties.apiVersion ) << "."
                          << VK_VERSION_PATCH( physicalDeviceProperties.apiVersion ) << endl;

  if( physicalDeviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu ) // Warn if a chosen device is not a GPU
    cerr << "Chosen physical device is not a descrete GPU!" << endl;

  vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures(); // Retrieve physical device Features
  if( !physicalDeviceFeatures.geometryShader )
    throw runtime_error("Chosen physical device doesn't have a geometry shader!"); // Geometry Shader is a required feature

  vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
  auto graphicsQFPIt = find_if( queueFamilyProperties.begin(), queueFamilyProperties.end(),
                                []( vk::QueueFamilyProperties const & qfp ) 
                                { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; } // find eGraphics queue family
                              );

  size_t graphicsQueueFamilyIndex = distance( queueFamilyProperties.begin(), graphicsQFPIt );
  size_t presentQueueFamilyIndex  = physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( graphicsQueueFamilyIndex ), surface ) 
                                    ? graphicsQueueFamilyIndex : queueFamilyProperties.size(); // check if the index has a surface support

#ifndef NDEBUG
  assert( presentQueueFamilyIndex == graphicsQueueFamilyIndex );
#endif

  float queuePriority = 0.0f;
  vk::DeviceQueueCreateInfo deviceQueueCreateInfo( vk::DeviceQueueCreateFlags(), static_cast<uint32_t>( graphicsQueueFamilyIndex ), 1, &queuePriority );

  vk::DeviceCreateInfo deviceCreateInfo( vk::DeviceCreateFlags(), deviceQueueCreateInfo, {}, deviceExtensions );
  vk::Device device             = physicalDevice.createDevice( deviceCreateInfo ); // "a GPU driver"
  vk::CommandPool commandPool   = device.createCommandPool( vk::CommandPoolCreateInfo( vk::CommandPoolCreateFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer ), graphicsQueueFamilyIndex ) );

  /* Actual objects used during rendering loop */
  vk::Queue graphicsQueue = device.getQueue( graphicsQueueFamilyIndex, 0 );
  vk::Queue presentQueue  = device.getQueue( presentQueueFamilyIndex, 0 );
  // vk::CommandBuffer commandBuffer = device.allocateCommandBuffers( vk::CommandBufferAllocateInfo( commandPool, vk::CommandBufferLevel::ePrimary, 1 ) ).front();
  vector<vk::CommandBuffer> commandBuffers = device.allocateCommandBuffers( vk::CommandBufferAllocateInfo( commandPool, vk::CommandBufferLevel::ePrimary, framesInFlight ) );
  
  vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR( surface ); // Retrieve supported formats
#ifndef NDEBUG
  assert( !formats.empty() );
#endif

  vk::Format format = ( formats[0] == vk::Format::eUndefined ) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;
  vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR( surface );
  vk::Extent2D swapchainExtent;
  if( surfaceCapabilities.currentExtent.width == numeric_limits<uint32_t>::max() ) // if the furface resolution is underfined
  { //... set it to the sizes of the image
    swapchainExtent.width  = clamp( static_cast<uint32_t>( width ),  surfaceCapabilities.minImageExtent.width,  surfaceCapabilities.maxImageExtent.width );
    swapchainExtent.height = clamp( static_cast<uint32_t>( height ), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height );
  } 
  else 
  { // if defined, the swapchain resolution must match it
    swapchainExtent = surfaceCapabilities.currentExtent;
  }

#ifndef NDEBUG
  cout << "Surface capabilities image count: min: " 
       << surfaceCapabilities.minImageCount 
       << ", max: " 
       << surfaceCapabilities.maxImageCount << endl; // FIXME maxImageCoutn is always equal to 0
#endif

  vk::PresentModeKHR swapchainPresentMode      = vk::PresentModeKHR::eFifo;
  vk::SurfaceTransformFlagBitsKHR preTransform = ( surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity )                                                                 ? vk::SurfaceTransformFlagBitsKHR::eIdentity // set to not apply any tranformation upon image
                                                  : surfaceCapabilities.currentTransform;
   
  vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque; // set to not blend with other windows
   
  vk::SwapchainCreateInfoKHR swapchainCreateInfo( vk::SwapchainCreateFlagsKHR(),
                                                  surface,
                                                  clamp( static_cast<uint32_t>(3), // define the number of image in the swapchain
                                                         surfaceCapabilities.minImageCount, 
                                                         static_cast<uint32_t>(3) /*surfaceCapabilities.maxImageCount*/ ), // FIXME 
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

  vk::SwapchainKHR swapchain = device.createSwapchainKHR ( swapchainCreateInfo ); // optional, used for rendering to the screen
  // NOTE: if the window resizes, swapchain must be recreated
                                                  
  vector<vk::Image> swapchainImages = device.getSwapchainImagesKHR( swapchain );
  vector<vk::ImageView> imageViews;
  imageViews.reserve( swapchainImages.size() );
  vk::ImageViewCreateInfo imageViewCreateInfo( {}, {}, vk::ImageViewType::e2D, format, {}, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } );
  for ( auto image : swapchainImages ) {
    imageViewCreateInfo.image = image;
    imageViews.push_back( device.createImageView( imageViewCreateInfo ) );
  }

  glslang::InitializeProcess();

  const string vertexShaderGLSLCode = R"(
    #version 450

    layout( location = 0 ) in vec2 inPosition; // location is an attribute position in VBO
    layout( location = 1 ) in vec3 inColor;

    layout( location = 0 ) out vec3 fragColor; 

    void main() {
       gl_Position = vec4( inPosition, 0.0, 1.0 );
       fragColor = inColor;
    }
  )";

  const string fragmentShaderGLSLCode = R"(
    #version 450

    layout(location = 0) in vec3 fragColor; // location = 0 is an index of the 0-th framebuffer

    layout(location = 0) out vec4 outColor;

    void main() {
      outColor = vec4(fragColor, 1.0);
    } 
  )";

  vector<unsigned int> vertexShaderSPV; // actual SPIR-V bytecode
  // string vertexShaderString = fileToString("../shader.vert");
  assert ( GLSLtoSPV( vk::ShaderStageFlagBits::eVertex, vertexShaderGLSLCode, vertexShaderSPV ) ); // Compiling GLSL vertex shader code to SPIR-V
  vk::ShaderModuleCreateInfo vertexShaderModuleCreateInfo( vk::ShaderModuleCreateFlags(), vertexShaderSPV );
  vk::ShaderModule vertexShaderModule = device.createShaderModule( vertexShaderModuleCreateInfo );

  vector<unsigned int> fragmentShaderSPV; // actual SPIR-V bytecode
  // string fragmentShaderString = fileToString("../shader.frag");
  assert( GLSLtoSPV( vk::ShaderStageFlagBits::eFragment, fragmentShaderGLSLCode, fragmentShaderSPV ) ); // Compiling GLSL fragment shader code to SPIR-V
  vk::ShaderModuleCreateInfo fragmentShaderModuleCreateInfo( vk::ShaderModuleCreateFlags(), fragmentShaderSPV );
  vk::ShaderModule fragmentShaderModule = device.createShaderModule( fragmentShaderModuleCreateInfo );

  glslang::FinalizeProcess();

  array<vk::PipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos = {
    vk::PipelineShaderStageCreateInfo( vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main" ),
    vk::PipelineShaderStageCreateInfo( vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main" )
  };

  array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor}; // setup viewport and scissor to be able to be changed during rendering
  vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo( vk::PipelineDynamicStateCreateFlags(), dynamicStates );

  vk::VertexInputBindingDescription vertexInputBindingDescription( 0, sizeof( Vertex ), vk::VertexInputRate::eVertex ); // 1: index of binding in the array of bindings  
  array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescriptions = {
    vk::VertexInputAttributeDescription( 0, // binding 
                                         0, // location (in GLSL)
                                         vk::Format::eR32G32Sfloat, 
                                         offsetof( Vertex, pos ) ), // 0  -- offset in bytes
    vk::VertexInputAttributeDescription( 1, 
                                         0, // location (in GLSL)
                                         vk::Format::eR32G32B32Sfloat, 
                                         offsetof( Vertex, color ) )
  };

  vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo( vk::PipelineVertexInputStateCreateFlags(),
                                                                             vertexInputBindingDescription,
                                                                             vertexInputAttributeDescriptions );
  vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo( vk::PipelineInputAssemblyStateCreateFlags(), 
                                                                                 vk::PrimitiveTopology::eTriangleList ); // specify what kind of geometry will be drawn from vertices
  
  vk::Buffer vertexBuffer = device.createBuffer( vk::BufferCreateInfo ( vk::BufferCreateFlags(), sizeof( vertices[0] ) * vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer ) );
  vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements( vertexBuffer );
  vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
  uint32_t memoryTypeIndex = uint32_t( ~0 );
  vk::MemoryPropertyFlags requirementsMask;
  for( uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++ )
  {
    if ( ( memoryRequirements.memoryTypeBits & 1 ) && ( ( memoryProperties.memoryTypes[i].propertyFlags & requirementsMask ) == requirementsMask ) )
    {
      memoryTypeIndex = i;
      break;
    }
    memoryRequirements.memoryTypeBits >>= 1;
  }
  assert( memoryTypeIndex != uint32_t( ~0 ) );
#ifndef NDEBUG
  cout << "memoryTypeIndex = " << memoryTypeIndex << endl;
#endif
  vk::DeviceMemory deviceMemory = device.allocateMemory( vk::MemoryAllocateInfo( memoryRequirements.size, memoryTypeIndex ) );
  device.bindBufferMemory( vertexBuffer, deviceMemory, 0 );
  /* actually allocating memory to GPU */
  uint8_t* deviceData = static_cast<uint8_t*>( device.mapMemory( deviceMemory, 0, sizeof( vertices[0] ) * vertices.size() ) );
  memcpy( deviceData, vertices.data(), sizeof( vertices[0] ) * vertices.size() );
  device.unmapMemory( deviceMemory );

  vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f);
  vk::Rect2D scissor({0, 0}, swapchainExtent);
  vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo( vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor);
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
                                                                                 1.0f );  // lineWidth
  
  vk::PipelineMultisampleStateCreateInfo pipelineMultisaampleStateCreateInfo(  // multisampling setup (one of ways to preform anti-aliasing)
                                                                               vk::PipelineMultisampleStateCreateFlags(), 
                                                                               vk::SampleCountFlagBits::e1,
                                                                               false, // sampleShadingEnable
                                                                               1.0f, // minSampleShading
                                                                               nullptr, // pSampleMask
                                                                               false, // alphaToCoverageEnable
                                                                               false ); // alphaToOneEnable
 
  vk::StencilOpState stencilOpState( vk::StencilOp::eKeep, 
                                     vk::StencilOp::eKeep, 
                                     vk::StencilOp::eKeep, 
                                     vk::CompareOp::eAlways ); // the rest is optional
  
  vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo( vk::PipelineDepthStencilStateCreateFlags(),
                                                                               true, // depthTestEnable
                                                                               true, // depthWriteEnable
                                                                               vk::CompareOp::eLessOrEqual, // depthCompareOp
                                                                               false, // depthBoundTestEnable
                                                                               false, // stencilTestEnable
                                                                               stencilOpState, // front
                                                                               stencilOpState ); // back
  
  vk::ColorComponentFlags colorComponentFlags( vk::ColorComponentFlagBits::eR | 
                                               vk::ColorComponentFlagBits::eG | 
                                               vk::ColorComponentFlagBits::eB | 
                                               vk::ColorComponentFlagBits::eA );

  vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState( false, // blendEnable
                                                                           vk::BlendFactor::eZero, // srcColorBlendFactor
                                                                           vk::BlendFactor::eZero, // sdtColorBlendFactro
                                                                           vk::BlendOp::eAdd, // colorBlendOp
                                                                           vk::BlendFactor::eZero, // srcAlphaBlendFactor
                                                                           vk::BlendFactor::eZero, // dstAlphaBlendFactor
                                                                           vk::BlendOp::eAdd, // AlphaBlendOp
                                                                           colorComponentFlags );
                                                                          
  vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo( vk::PipelineColorBlendStateCreateFlags(),
                                                                           false, // logicOpEnable
                                                                           vk::LogicOp::eNoOp, // logicOp
                                                                           pipelineColorBlendAttachmentState, // attachments
                                                                           { {1.0f, 1.0f, 1.0f, 1.0f } } ); // blendContrasts

  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo( // used to specify global variables which will be used by the pipeline, e.g. transformation matrices
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
  attachmentDescriptions.emplace_back( vk::AttachmentDescriptionFlags(), 
                                       format, // we take from a swapchain color format
                                       vk::SampleCountFlagBits::e1,
                                       vk::AttachmentLoadOp::eClear,
                                       vk::AttachmentStoreOp::eStore,
                                       vk::AttachmentLoadOp::eDontCare, // stencil loagOp
                                       vk::AttachmentStoreOp::eDontCare, // stencil storeOp
                                       vk::ImageLayout::eUndefined,
                                       vk::ImageLayout::ePresentSrcKHR );

  vk::AttachmentReference colorAttachment( 0, vk::ImageLayout::eColorAttachmentOptimal );
  // vk::AttachmentReference depthAttachment( 1, vk::ImageLayout::eDepthStencilAttachmentOptimal ); // TODO
  vk::SubpassDescription subpassDescription( vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, {}, colorAttachment, {}, nullptr);
  vk::SubpassDependency dependency( VK_SUBPASS_EXTERNAL, 
                                    0, 
                                    vk::PipelineStageFlagBits::eColorAttachmentOutput, 
                                    vk::PipelineStageFlagBits::eColorAttachmentOutput, 
                                    vk::AccessFlagBits::eNone, 
                                    vk::AccessFlagBits::eColorAttachmentWrite );
  vk::RenderPass renderPass = device.createRenderPass( vk::RenderPassCreateInfo( vk::RenderPassCreateFlags(), attachmentDescriptions, subpassDescription, dependency ) );
  // NOTE: it is impossible to do rendering commands outside of the renderPass,
  //       but it's possible to do compute commands without it 


  vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(  vk::PipelineCreateFlags(),
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
                                                              renderPass );
  auto [result, pipeline] = device.createGraphicsPipeline( nullptr, graphicsPipelineCreateInfo );
  // NOTE: the pipeline object is like a GPU code of the rendering pipeline
  if( result != vk::Result::eSuccess )
    throw runtime_error( "Couldn't create graphics pipeline!" );

  vector<vk::Framebuffer> swapchainFramebuffers;
  for(auto const & imageView : imageViews) 
  {
    vk::ImageView             attachment = { imageView };
    vk::FramebufferCreateInfo framebufferCreateInfo( vk::FramebufferCreateFlags(), 
                                                     renderPass, 
                                                     attachment, 
                                                     swapchainExtent.width, 
                                                     swapchainExtent.height, 
                                                     1 );
    swapchainFramebuffers.push_back( device.createFramebuffer( framebufferCreateInfo) );
  }

  /* Preparing for rendering */
  array<vk::ClearValue, 2> clearValues; // defining clear value for vk::AttachmentLoadOp::eClear mentioned above
  array<float, 4> clearColorValues = { 0.0f, 0.0f, 0.0f, 0.0f };
  clearValues[0].color        = vk::ClearColorValue( clearColorValues );
  clearValues[1].depthStencil = vk::ClearDepthStencilValue( 1.0f, 0 );

  // NOTE: Implementing rendering and presentation concurrently
  vector<vk::Semaphore> imageAvailableSemaphores( framesInFlight, device.createSemaphore( vk::SemaphoreCreateInfo() ) );
  vector<vk::Semaphore> renderFinishedSemaphores( framesInFlight, device.createSemaphore( vk::SemaphoreCreateInfo() ) );
  vector<vk::Fence>     inFlightFences( framesInFlight , device.createFence( vk::FenceCreateInfo( vk::FenceCreateFlagBits::eSignaled ) ) );

  const uint64_t timeout = numeric_limits<uint64_t>::max();

  /* Main Rendering Loop */
  uint32_t currentFrame = 0;
  bool quit = false;
  while( !quit )
  { 
     SDL_Event event;
     SDL_PollEvent( &event );
     if( event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE ) quit = true;

     /* Srating to draw */
     device.waitForFences( inFlightFences[currentFrame], VK_TRUE, timeout ); // 2: wait for all fences (if one fence -- douesn't matter), 3: fenceTimeout
     device.resetFences( inFlightFences[currentFrame] ); // unsignal fence

     vk::ResultValue<uint32_t> currentBuffer = device.acquireNextImageKHR( swapchain, // current swapchain image
                                                                           timeout, 
                                                                           imageAvailableSemaphores[currentFrame], // signalSemaphores
                                                                           nullptr );
#ifndef NDEBUG
     assert( currentBuffer.result == vk::Result::eSuccess );
#endif

     // TODO: resetting the pool instead of te buffer is quicker if you have multiple buffers per pool
     // NOTE: resetting the buffer is unsafe when it is still in submition DynamicState
     //       that's why inFlightFence wait command is used above    
     commandBuffers[currentFrame].reset(); // clear the buffer
     // commandPool.reset( vk::CommandPoolResetFlags ( vk::CommandPoolResetFlagBits::eReleaseResources ) ); // clear buffers

     commandBuffers[currentFrame].begin( vk::CommandBufferBeginInfo( vk::CommandBufferUsageFlags() ) ); // start a buffer
     vk::RenderPassBeginInfo renderPassBeginInfo( renderPass, 
                                                  swapchainFramebuffers[currentBuffer.value], 
                                                  vk::Rect2D( vk::Offset2D( 0, 0 ), swapchainExtent ), 
                                                  clearValues );
     commandBuffers[currentFrame].beginRenderPass( renderPassBeginInfo, vk::SubpassContents::eInline ); // all commands submit to primary command buffer
     commandBuffers[currentFrame].bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline ); // first parameter specifies whether the pipeline is graphical or computational
     
     commandBuffers[currentFrame].bindVertexBuffers( 0, vertexBuffer, { 0 } ); // since we made viewport and scissor to be dynamic, we have to specify them now
     commandBuffers[currentFrame].setViewport( 0, vk::Viewport( 0.0f, 0.0f, static_cast<float>( swapchainExtent.width ), static_cast<float>( swapchainExtent.height ), 0.0f, 1.0f ) );
     commandBuffers[currentFrame].setScissor( 0, vk::Rect2D( vk::Offset2D( 0, 0 ), swapchainExtent ) );
     // loading command to commandBuffer for execution
     commandBuffers[currentFrame].draw( static_cast<uint32_t>( vertices.size() ), // number of vertices
                                        1, // used for instanced drawing
                                        0, // firstVertex offset
                                        0 ); // firstInstance offset
     // after drawing we can now end renderPass and commandBuffer
     commandBuffers[currentFrame].endRenderPass();
     commandBuffers[currentFrame].end();
     vk::PipelineStageFlags waitDestinationStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
     vk::SubmitInfo submitInfo( imageAvailableSemaphores[currentFrame], // waitSemaphores
                                waitDestinationStageMask, 
                                commandBuffers[currentFrame], 
                                renderFinishedSemaphores[currentFrame] ); // signalSemaphores
     graphicsQueue.submit( submitInfo, inFlightFences[currentFrame] );
     presentQueue.presentKHR( vk::PresentInfoKHR( renderFinishedSemaphores[currentFrame], swapchain, currentBuffer.value ) );
     // quit = true;
     currentFrame = (currentFrame + 1) % framesInFlight;
  }
  /* when exiting main loop drawing and presentation operation may still be going and destroying its resources is a bad idea. Solution: */
  device.waitIdle(); // wait until all GPU operations complete

   // cleanup 
  for(int i = 0; i < framesInFlight; ++i)
  {
     device.destroyFence( inFlightFences[i] );
     device.destroySemaphore( imageAvailableSemaphores[i] );
     device.destroySemaphore( renderFinishedSemaphores[i] );
  }
  device.freeCommandBuffers( commandPool, commandBuffers );
  device.destroyCommandPool( commandPool );
  for(auto const & swapchainFramebuffer : swapchainFramebuffers)
    device.destroyFramebuffer( swapchainFramebuffer );
  device.destroyBuffer( vertexBuffer );
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
  instance.destroy();
  return EXIT_SUCCESS;
} 
catch(SDL2pp::Exception& e) 
{
  cerr << "Error in: " << e.GetSDLFunction() << endl;
  cerr << "   Reason: " << e.GetSDLError() << endl;
  return EXIT_FAILURE;
} 
catch(const exception& e) 
{
  cerr << e.what() << endl;
  return EXIT_FAILURE;
} 
