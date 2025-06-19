#pragma once
#include "VulkanHeader.h"
#include <string>

namespace CHIKU
{
	class Renderer
	{
	public:
		static void Init();
		static void Draw();
		static void CleanUp();
	};
}