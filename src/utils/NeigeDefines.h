#pragma once
#include "vulkan/vulkan.hpp"
#include "../../external/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <string>

#define NEIGE_VK_CHECK(f) \
	do { \
		VkResult check = f; \
		if (check) { \
			std::cerr << "\033[1m\033[31mVULKAN ERROR : \033[39m\033[0m A Vulkan error has happened." << std::endl << "File : " << __FILE__ << std::endl << "Function : " << __FUNCTION__ << std::endl << "Line : " << __LINE__ << std::endl; \
			exit(1); \
		} \
	} while (0)

#define NEIGE_VK_VALIDATION_ERROR(m) \
	do { \
		std::cerr << "\033[1m\033[31mVALIDATION LAYER : \033[39m\033[0m" << m << std::endl; \
	} while(0)

#define NEIGE_WARNING(m) \
	do { \
		std::cerr << "\033[1m\033[33mNEIGE WARNING : \033[39m\033[0m" << m << std::endl; \
	} while(0)

#define NEIGE_ERROR(m) \
	do { \
		std::cerr << "\033[1m\033[31mNEIGE ERROR : \033[39m\033[0m" << m << std::endl; \
		exit(2); \
	} while(0)

#ifndef NDEBUG
#define NEIGE_INFO(m) \
	do { \
		std::cout << "\033[1m\033[36mNEIGE INFO : \033[39m\033[0m" << m << std::endl; \
	} while(0)
#else
#define NEIGE_INFO(m) \
	do { \
	} while(0)
#endif

#ifndef NDEBUG
#define NEIGE_ASSERT(c, m) \
		do { \
			if (!c) { \
				std::cerr << "\033[1m\033[35mNEIGE ASSERT : \033[39m\033[0m" << m << std::endl; \
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