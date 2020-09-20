#pragma once
#include <glm/glm.hpp>

//composition over inheretance.
//it's cumbersome to write mySpotlight.point_values.basic_values.color.x
//but the benefits of garenteed data layout might be important later
//and i'm willing to trade for that bc i can press tab for code completion

struct BasicLight {
	glm::vec3 color;
	float diffuse_strength;
};

struct PointLight {
	BasicLight basic_values;
	glm::vec3 position;
	struct {
		float constant;
		float linear;
		float quadratic;
	}attenuation;
};

struct VolumeLight {
	BasicLight basic_values;
	glm::vec3 position;
	struct {
		float constant;
		float linear;
		float quadratic;
	}attenuation;
	float radius;
};

struct SpotLight {
	PointLight point_values;
	glm::vec3 direction;
	float cutoff;

};

class LightManager {
public:

	static void calculate_radius(VolumeLight &p) {
		const float max_brightness = std::fmaxf(std::fmaxf(p.basic_values.color.r, p.basic_values.color.g), p.basic_values.color.b);
		p.radius = (-p.attenuation.linear + std::sqrt(p.attenuation.linear * p.attenuation.linear - 4
			* p.attenuation.quadratic * (p.attenuation.constant - (256.0f / 5.0f) * max_brightness))) / (2.0f * p.attenuation.quadratic);
	}

	static SpotLight create_spotlight(glm::vec3 color = { 0.0f,0.0f,0.0f }, float diffuse = 0.5f,
		glm::vec3 position = { 0.0f, 0.0f, 0.0f }, float attenuation_c = 1.0f, float attenuation_l = 0.0f,
		float attenuation_q = 0.0f, glm::vec3 direction = {0.0f, 1.0f, 0.0f}, float cutoff = 45.0f) {
		SpotLight sl = { {{color, diffuse}, position, {attenuation_c, attenuation_l, attenuation_q}}, direction, cutoff };
		return sl;
	}

	static PointLight create_pointlight(glm::vec3 color = { 0.0f,0.0f,0.0f }, float diffuse = 0.5f,
		glm::vec3 position = { 0.0f, 0.0f, 0.0f }, float attenuation_c = 1.0f, float attenuation_l = 0.0f,
		float attenuation_q = 0.0f) {
		PointLight pl = { {color, diffuse}, position, {attenuation_c, attenuation_l, attenuation_q}};
		return pl;

	}

	static VolumeLight create_volumelight(glm::vec3 color = { 0.0f,0.0f,0.0f }, float diffuse = 0.5f,
		glm::vec3 position = { 0.0f, 0.0f, 0.0f }, float attenuation_c = 1.0f, float attenuation_l = 0.0f,
		float attenuation_q = 0.0f) {
		VolumeLight pl = { {color, diffuse}, position, {attenuation_c, attenuation_l, attenuation_q}, 0.0f };
		calculate_radius(pl);
		return pl;

	}

	static BasicLight create_basiclight(glm::vec3 color = {1.0f, 1.0f, 1.0f}, float diffuse = 0.5f) {
		BasicLight bl = {color, diffuse};
		return bl;
	}
};

