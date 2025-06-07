#pragma once
#include "VulkanHeader.h"
#include <random>
#include <cstdint>

namespace CHIKU
{
	namespace Utils
	{
		static std::random_device  m_RandomDevice;
		static std::mt19937_64 m_Range;

		template<typename T>
		T GetRandomNumber()
		{
			ZoneScoped;

			std::uniform_int_distribution<uint64_t> dist;
			return dist(m_Range);
		}
	}
}