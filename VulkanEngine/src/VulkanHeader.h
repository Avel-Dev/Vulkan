#pragma once

#define SOURCE_DIR std::string(STR(CHIKU_SRC_PATH))
#define ASSET_REGISTRY SOURCE_DIR + std::string(STR(Registry/AssetRegistry.yaml)) 

#ifdef PLT_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32 
#define NOMINMAX  // Add this before glfw3native.h to suppress min/max macros
#elif PLT_UNIX
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#define ENABLE_VALIDATION_LAYERS

#define MAX_TEXTURE_PER_MATERIAL 5
#define MAX_FRAMES_IN_FLIGHT 3
#define MAX_DESCRIPTOR_SETS 1000
#define MAX_DESCRIPTOR_SET_LAYOUTS 1000

#define STR2(x) #x
#define STR(x) STR2(x)

#include <tracy/tracy.hpp>
#include <tracy/tracyC.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <cstring>
#include <array>
#include <unordered_map>
