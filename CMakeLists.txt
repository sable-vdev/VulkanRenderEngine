#honestly no idea if it still works, complete file made by chatgpt
cmake_minimum_required(VERSION 3.10)
project(VulkanRenderEngine VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20) #due to that one std function
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# Specify the main source directory
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/VulkanRenderEngine")

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Include GLM (Header-only library)
find_path(GLM_INCLUDE_DIRS glm/glm.hpp)

# Add source files
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")

# Include directories
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${SOURCE_DIR})

# Create the executable
add_executable(VulkanRenderEngine ${SOURCES})

# Link libraries
target_link_libraries(VulkanRenderEngine PRIVATE Vulkan::Vulkan glfw)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
