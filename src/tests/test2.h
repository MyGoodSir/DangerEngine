#pragma once
#include "Scene.h"
namespace DGR {
	class test2 : public Scene
	{
	public:
		uint NUM_LIGHTS = 32;
		Framebuffer fb;
		Shader shader_geom_pass;
		Shader shader_lighting_pass;
		Shader shader_light_box;
		Model backpack, block;
		std::vector<glm::vec3> instance_pos;
		std::vector<VolumeLight> lights;
		void setup();
		void render();
		void renderCube();
		void renderQuad();
	};

}