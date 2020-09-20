#include "test2.h"

namespace DGR {
	void test2::setup() {
		genWindow(800, 600, "Danger Renderer");

		initScene();

		mainCamera = CameraManager::Create(glm::vec3(0.0f, 0.0f, 5.0f));
		shader_geom_pass = ShaderManager::Generate("src/shaders/gbuff.vert", "src/shaders/gbuff.frag");
		shader_lighting_pass = ShaderManager::Generate("src/shaders/deferred.vert", "src/shaders/deferred.frag");
		shader_light_box = ShaderManager::Generate("src/shaders/light_box.vert", "src/shaders/light_box.frag");
		backpack = ModelManager::Create("src/assets/models/backpack/backpack.obj");
		block = ModelManager::Create("src/assets/models/Box/box.obj");
		instance_pos.push_back(glm::vec3(-3.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(0.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(3.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(-3.0, -0.5, 0.0));
		instance_pos.push_back(glm::vec3(0.0, -0.5, 0.0));
		instance_pos.push_back(glm::vec3(3.0, -0.5, 0.0));
		instance_pos.push_back(glm::vec3(-3.0, -0.5, 3.0));
		instance_pos.push_back(glm::vec3(0.0, -0.5, 3.0));
		instance_pos.push_back(glm::vec3(3.0, -0.5, 3.0));
		fb = FramebufferManager::create(WINDOW_WIDTH, WINDOW_HEIGHT);
		FBTexture gPos{ GL_RGBA, GL_RGBA16F, GL_FLOAT };
		FBTexture gNorm{ GL_RGBA, GL_RGBA16F, GL_FLOAT };
		FBTexture gAlbSpec{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE };
		FramebufferManager::create_fbtexture(fb, gPos);
		FramebufferManager::create_fbtexture(fb, gNorm);
		FramebufferManager::create_fbtexture(fb, gAlbSpec);
		FramebufferManager::draw_target_all(fb);
		FramebufferManager::gen_renderbuffer(fb);

		srand(13);
		for (unsigned int i = 0; i < NUM_LIGHTS; i++)
		{
			float x = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float y = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			float z = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float r = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float g = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float b = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			const float constant = 1.0;
			const float linear = 0.7;
			const float quadratic = 1.8;
			lights.push_back(
				LightManager::create_volumelight({ r, g, b }, 0, { x, y, z }, constant, linear, quadratic)
			);
		}

		ShaderManager::use(shader_lighting_pass);
		ShaderManager::setUniInt(shader_lighting_pass, "gPosition", 0);
		ShaderManager::setUniInt(shader_lighting_pass, "gNormal", 1);
		ShaderManager::setUniInt(shader_lighting_pass, "gAlbedoSpec", 2);

	}
	void test2::render() {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput();


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// 1. geometry pass: render scene's geometry/color data into gbuffer
		FramebufferManager::bind(fb);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(mainCamera.zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = CameraManager::GetViewMatrix(mainCamera);
		glm::mat4 model = glm::mat4(1.0f);
		ShaderManager::use(shader_geom_pass);
		ShaderManager::setUniMat4(shader_geom_pass, "projection", projection);
		ShaderManager::setUniMat4(shader_geom_pass, "view", view);
		for (unsigned int i = 0; i < instance_pos.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, instance_pos[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			ShaderManager::setUniMat4(shader_geom_pass, "model", model);
			ModelManager::Draw(backpack, shader_geom_pass);
		}
		//using a box for the ground
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -26.0, 0.0));
		model = glm::scale(model, glm::vec3(25.0f));
		ShaderManager::setUniMat4(shader_geom_pass, "model", model);
		ModelManager::Draw(block, shader_geom_pass);
		FramebufferManager::unbind();

		// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::use(shader_lighting_pass);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fb.attachments[0].handle);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fb.attachments[1].handle);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, fb.attachments[2].handle);



		for (unsigned int i = 0; i < lights.size(); i++)
		{
			ShaderManager::setUniVec3(shader_lighting_pass, "lights[" + std::to_string(i) + "].Position", lights[i].position);
			ShaderManager::setUniVec3(shader_lighting_pass, "lights[" + std::to_string(i) + "].Color", lights[i].basic_values.color);
			ShaderManager::setUniFloat(shader_lighting_pass, "lights[" + std::to_string(i) + "].Linear", lights[i].attenuation.linear);
			ShaderManager::setUniFloat(shader_lighting_pass, "lights[" + std::to_string(i) + "].Quadratic", lights[i].attenuation.quadratic);
			ShaderManager::setUniFloat(shader_lighting_pass, "lights[" + std::to_string(i) + "].Radius", lights[i].radius);
		}
		ShaderManager::setUniVec3(shader_lighting_pass, "viewPos", mainCamera.pos);
		renderQuad();

		// 2.5. copy geom depth buffer to default depth buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.handle);//read from
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. render lights
		ShaderManager::use(shader_light_box);
		ShaderManager::setUniMat4(shader_light_box, "projection", projection);
		ShaderManager::setUniMat4(shader_light_box, "view", view);
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, lights[i].position);
			model = glm::scale(model, glm::vec3(0.125f));
			ShaderManager::setUniMat4(shader_light_box, "model", model);
			ShaderManager::setUniVec3(shader_light_box, "lightColor", lights[i].basic_values.color);
			renderCube();
		}




		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	void test2::renderCube()
	{
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				// bottom face
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}


	// renderQuad() renders a 1x1 XY quad in NDC
	// -----------------------------------------
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void test2::renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}