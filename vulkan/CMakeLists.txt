cmake_minimum_required(VERSION 3.13)
project(vulkan_tutorial)
set(CMAKE_CXX_STANDARD 20)
find_package(SDL2pp REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
add_executable(${PROJECT_NAME} vulkan_tutorial.cpp)
target_compile_options(${PROJECT_NAME} 
                       PUBLIC -fsanitize=leak -g -Wall -Wextra -O2)
if(Debug)
   message("Configuring Makefile to compile with NDEBUG set")
   target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
endif()
target_link_libraries(${PROJECT_NAME} 
                      PRIVATE SDL2pp::SDL2pp 
                      Vulkan::Vulkan
                      glm::glm)
