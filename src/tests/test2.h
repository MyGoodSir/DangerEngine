#pragma once
#include "Scene.h"
namespace DGR {
	class test2 : public Scene
	{
	public:
		uint NUM_LIGHTS = 32;//number of light volumes to be rendered
		Framebuffer fb;
		Shader gbuff_pass;
		Shader lighting_pass;
		Shader light_geometry;
		Model backpack, block;
		std::vector<glm::vec3> instance_pos; //model instance data
		std::vector<VolumeLight> lights; //light volume data
		void setup();
		void render();
		void renderCube();
		void renderQuad();
	};

}