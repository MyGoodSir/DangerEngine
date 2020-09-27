#pragma once
#include "util/typealias.h"
#include <glm/glm.hpp>
#include "camera.h"

struct PMVertex {
	glm::vec3 position;
	glm::vec2 texture_coordinates;
	
};

//mesh data
struct PortalMesh {
	std::vector<PMVertex> verts;
	uint vao, vbo;
};

struct Portal {
	Camera cam;
	glm::mat4 modelMatrix;
	PortalMesh mesh;
	Portal* sister;

};

struct PortalManager {

	static Portal create() {
		Portal p = { CameraManager::Create(), glm::mat4(1.0f) };

	}

	void draw(Portal& p)
	{
		if (p.mesh.vao == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &p.mesh.vao);
			glGenBuffers(1, &p.mesh.vbo);
			glBindVertexArray(p.mesh.vao);
			glBindBuffer(GL_ARRAY_BUFFER, p.mesh.vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(p.mesh.vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
};