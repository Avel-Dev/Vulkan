#pragma once
#include <glm/glm.hpp>
#include "GraphicsPipeline.h"

namespace CHIKU
{
	class Renderer
	{
	public:
		void Init();             
		void Shutdown();
		void BeginFrame();
		void EndFrame();
		void BeginScene();
		void EndScene();

		void BindGraphicsPipeline(GraphicsPipeline pipeline);

		void DrawMesh(const Mesh& mesh, const Material& material, const glm::mat4& transform);
		void DrawModel(const Model& model, const glm::mat4& transform); 

		void BeginShadowPass();
		void EndShadowPass();

		void BeginPostProcessing();
		void EndPostProcessing();
	};
}