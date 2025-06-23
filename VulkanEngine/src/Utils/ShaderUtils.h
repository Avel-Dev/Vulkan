#pragma once
#include <spirv_reflect.h>
#include <vector>
#include <string>	

namespace CHIKU
{
	namespace Utils 
	{
		std::vector<uint32_t> LoadSPIRV(const std::string& path);
	}
}