cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
SET(NAME NeigeEngine)
PROJECT(${NAME} VERSION 0.0.1 LANGUAGES CXX)

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
link_libraries(glfw)

IF (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	message(STATUS "Looking for Vulkan...")
	find_package(Vulkan)
ENDIF()

IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ELSE()
	message(STATUS ${Vulkan_LIBRARY})
	link_libraries(Vulkan::Vulkan)
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-enum")
ENDIF()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(GAME_SOURCES src/Game.cpp)
SET(GAME_HEADERS src/Game.h)
SET(GRAPHICS_SOURCES src/graphics/CommandBuffer.cpp src/graphics/CommandPool.cpp src/graphics/Framebuffer.cpp src/graphics/Image.cpp src/graphics/Instance.cpp src/graphics/LogicalDevice.cpp src/graphics/PhysicalDevice.cpp src/graphics/Renderer.cpp src/graphics/RenderPass.cpp src/graphics/RenderPassAttachment.cpp src/graphics/Swapchain.cpp)
SET(GRAPHICS_HEADERS src/graphics/CommandBuffer.h src/graphics/CommandPool.h src/graphics/Framebuffer.h src/graphics/Image.h src/graphics/Instance.h src/graphics/LogicalDevice.h src/graphics/PhysicalDevice.h src/graphics/Renderer.h src/graphics/RenderPass.h src/graphics/RenderPassAttachment.h src/graphics/Swapchain.h)
SET(UTILS_HEADERS src/utils/FileTools.cpp src/utils/ImageTools.h src/utils/MemoryAllocator.h src/utils/NeigeDefines.h src/utils/NeigeStructs.h src/utils/RendererResources.h)
SET(UTILS_SOURCES src/utils/FileTools.h src/utils/ImageTools.cpp src/utils/MemoryAllocator.cpp)
SET(WINDOW_SOURCES src/window/Surface.cpp src/window/Window.cpp)
SET(WINDOW_HEADERS src/window/Surface.h src/window/Window.h)
SET(SOURCES ${GAME_SOURCES} ${GRAPHICS_SOURCES} ${WINDOW_SOURCES} ${UTILS_SOURCES})
SET(HEADERS ${GAME_HEADERS} ${GRAPHICS_HEADERS} ${WINDOW_HEADERS} ${UTILS_HEADERS})

add_executable(${PROJECT_NAME} main.cpp ${SOURCES} ${HEADERS})