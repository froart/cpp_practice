cmake_minimum_required( VERSION 3.13 )
project( vulkan_tutorial )
set( CMAKE_CXX_STANDARD 20 )
find_package( SDL2pp REQUIRED )
find_package( glm REQUIRED )
find_package( Vulkan REQUIRED )

add_executable( ${PROJECT_NAME} vulkan_tutorial.cpp )
if( NOT Debug )
   message("Configuring Makefile in NON-debugging mode")
   target_compile_definitions( ${PROJECT_NAME} PRIVATE 
                                 NDEBUG )
   target_compile_options( ${PROJECT_NAME} PUBLIC 
                             -O2 )
else()
   message("Configuring Makefile in debugging mode")
   target_compile_options( ${PROJECT_NAME} PUBLIC 
                             -fsanitize=leak 
                             -g 
                             -Wall 
                             -Wextra )
endif()

target_link_libraries( ${PROJECT_NAME} PUBLIC 
                       SDL2pp::SDL2pp 
                       Vulkan::Vulkan
                       glm::glm )
# without the libraries below doesn't compile
target_link_libraries( ${PROJECT_NAME} PUBLIC MachineIndependent ) 
target_link_libraries( ${PROJECT_NAME} PUBLIC OSDependent ) 
target_link_libraries( ${PROJECT_NAME} PUBLIC GenericCodeGen ) 
target_link_libraries( ${PROJECT_NAME} PUBLIC OGLCompiler ) 
target_link_libraries( ${PROJECT_NAME} PUBLIC glslang ) 
target_link_libraries( ${PROJECT_NAME} PUBLIC glslang-default-resource-limits ) 
target_link_libraries( ${PROJECT_NAME} PUBLIC SPIRV ) 
# These two must be added in this particular order
target_link_libraries( ${PROJECT_NAME} PUBLIC SPIRV-Tools-opt )
target_link_libraries( ${PROJECT_NAME} PUBLIC SPIRV-Tools ) 

