#pragma once
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <string>

#define NEIGE_VK_CHECK(f) \
	do { \
		VkResult check = f; \
		if (check) { \
			std::cerr << "A Vulkan error has happened in file " << __FILE__ << " at line " << __LINE__ << "." << std::endl; \
			exit(1); \
		} \
	} while (0)

#define NEIGE_WARNING(m) \
	do { \
		std::cerr << m << std::endl; \
	} while(0)

#define NEIGE_ERROR(m) \
	do { \
		std::cerr << m << std::endl; \
		exit(2); \
	} while(0)

#define NEIGE_INFO(m) \
	do { \
		std::cout << m << std::endl; \
	} while(0)

#ifndef NDEBUG
#define NEIGE_ASSERT(c, m) \
		do { \
			if (!c) { \
				std::cerr << m << std::endl; \
				exit(3); \
			} \
		} while(0)
#else
#define NEIGE_ASSERT(c, m) \
		do { \
		} while(0)
#endif

#ifndef NDEBUG
const bool NEIGE_DEBUG = true;
#else
const bool NEIGE_DEBUG = false;
#endif