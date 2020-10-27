cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
SET(NAME NeigeEngine)
PROJECT(${NAME} VERSION 0.0.1 LANGUAGES CXX)

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include_directories(external/cgltf)
include_directories(external/glm)
include_directories(external/spirv-reflect)
include_directories(external/stb)

add_subdirectory(external/glfw)
link_libraries(glfw)

add_subdirectory(external/glslang)
link_libraries(glslang)
link_libraries(SPIRV)

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

SET(GRAPHICS_COMMANDS_SOURCES src/graphics/commands/CommandBuffer.cpp src/graphics/commands/CommandPool.cpp)
SET(GRAPHICS_COMMANDS_HEADERS src/graphics/commands/CommandBuffer.h src/graphics/commands/CommandPool.h)
SET(GRAPHICS_DEVICES_SOURCES src/graphics/devices/LogicalDevice.cpp src/graphics/devices/PhysicalDevice.cpp src/graphics/devices/PhysicalDevicePicker.cpp)
SET(GRAPHICS_DEVICES_HEADERS src/graphics/devices/LogicalDevice.h src/graphics/devices/PhysicalDevice.h src/graphics/devices/PhysicalDevicePicker.h)
SET(GRAPHICS_INSTANCE_SOURCES src/graphics/instance/Instance.cpp)
SET(GRAPHICS_INSTANCE_HEADERS src/graphics/instance/Instance.h)
SET(GRAPHICS_PIPELINES_SOURCES src/graphics/pipelines/DescriptorSet.cpp src/graphics/pipelines/GraphicsPipeline.cpp src/graphics/pipelines/Shader.cpp src/graphics/pipelines/Viewport.cpp)
SET(GRAPHICS_PIPELINES_HEADERS src/graphics/pipelines/DescriptorSet.h src/graphics/pipelines/GraphicsPipeline.h src/graphics/pipelines/Shader.h src/graphics/pipelines/Viewport.h)
SET(GRAPHICS_RENDERPASSES_SOURCES src/graphics/renderpasses/Framebuffer.cpp src/graphics/renderpasses/RenderPass.cpp src/graphics/renderpasses/RenderPassAttachment.cpp src/graphics/renderpasses/Swapchain.cpp)
SET(GRAPHICS_RENDERPASSES_HEADERS src/graphics/renderpasses/Framebuffer.h src/graphics/renderpasses/RenderPass.h src/graphics/renderpasses/RenderPassAttachment.h src/graphics/renderpasses/Swapchain.h)
SET(GRAPHICS_RESOURCES_SOURCES src/graphics/resources/Buffer.cpp src/graphics/resources/Image.cpp)
SET(GRAPHICS_RESOURCES_HEADERS src/graphics/resources/Buffer.h src/graphics/resources/Image.h src/graphics/resources/RendererResources.h)
SET(GRAPHICS_SYNC_SOURCES src/graphics/sync/Fence.cpp src/graphics/sync/Semaphore.cpp)
SET(GRAPHICS_SYNC_HEADERS src/graphics/sync/Fence.h src/graphics/sync/Semaphore.h)
SET(GRAPHICS_SOURCES src/graphics/Renderer.cpp ${GRAPHICS_COMMANDS_SOURCES} ${GRAPHICS_DEVICES_SOURCES} ${GRAPHICS_INSTANCE_SOURCES} ${GRAPHICS_PIPELINES_SOURCES} ${GRAPHICS_RENDERPASSES_SOURCES} ${GRAPHICS_RESOURCES_SOURCES} ${GRAPHICS_SYNC_SOURCES})
SET(GRAPHICS_HEADERS src/graphics/Renderer.h ${GRAPHICS_COMMANDS_HEADERS} ${GRAPHICS_DEVICES_HEADERS} ${GRAPHICS_INSTANCE_HEADERS} ${GRAPHICS_PIPELINES_HEADERS} ${GRAPHICS_RENDERPASSES_HEADERS} ${GRAPHICS_RESOURCES_HEADERS} ${GRAPHICS_SYNC_HEADERS})

SET(UTILS_MEMORYALLOCATOR_SOURCES src/utils/memoryallocator/MemoryAllocator.cpp)
SET(UTILS_MEMORYALLOCATOR_HEADERS src/utils/memoryallocator/MemoryAllocator.h)
SET(UTILS_RESOURCES_SOURCES src/utils/resources/BufferTools.cpp src/utils/resources/FileTools.cpp src/utils/resources/ImageTools.cpp src/utils/resources/ModelLoader.cpp)
SET(UTILS_RESOURCES_HEADERS src/utils/resources/BufferTools.h src/utils/resources/FileTools.h src/utils/resources/ImageTools.h src/utils/resources/ModelLoader.h)
SET(UTILS_STRUCTS_HEADERS src/utils/structs/RendererStructs.h src/utils/structs/ShaderStructs.h)
SET(UTILS_SOURCES src/utils/NeigeVKTranslate.cpp ${UTILS_MEMORYALLOCATOR_SOURCES} ${UTILS_RESOURCES_SOURCES})
SET(UTILS_HEADERS src/utils/NeigeDefines.h src/utils/NeigeVKTranslate.h ${UTILS_MEMORYALLOCATOR_HEADERS} ${UTILS_RESOURCES_HEADERS} ${UTILS_STRUCTS_HEADERS})

SET(WINDOW_SOURCES src/window/Surface.cpp src/window/Window.cpp)
SET(WINDOW_HEADERS src/window/Surface.h src/window/Window.h)

SET(ECS_COMPONENTS_HEADERS src/ecs/components/Camera.h src/ecs/components/Transform.h)
SET(ECS_HEADERS src/ecs/ECS.h ${ECS_COMPONENTS_HEADERS})

SET(EXTERNAL_SOURCES external/spirv-reflect/spirv_reflect.c)

SET(SOURCES ${GAME_SOURCES} ${GRAPHICS_SOURCES} ${UTILS_SOURCES} ${WINDOW_SOURCES} ${EXTERNAL_SOURCES})
SET(HEADERS ${GAME_HEADERS} ${GRAPHICS_HEADERS} ${UTILS_HEADERS} ${WINDOW_HEADERS} ${ECS_HEADERS})

add_executable(${PROJECT_NAME} main.cpp ${SOURCES} ${HEADERS})