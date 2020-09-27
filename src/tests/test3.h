#pragma once
#include "Scene.h"
namespace DGR {
	class test3 : public Scene
	{
	public:
		uint NUM_LIGHTS = 1;
		Shader shader_bp, shader_portal_pass;
		Model backpack, block, panel;
		std::vector<glm::vec3> instance_pos;
		std::vector<VolumeLight> lights;
		glm::vec3 p1_pos, p2_pos;
		Camera p1_cam, p2_cam;
		unsigned int quadVAO = 0;
		unsigned int quadVBO;
		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;
		//bool show_demo_window = false;
		//bool show_another_window = false;
		ImVec4 light_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGuiIO* io;
		void setup();
		void render();
		void renderCube();
		void renderQuad();
	};

}

