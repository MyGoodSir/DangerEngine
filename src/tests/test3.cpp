#include "test3.h"

namespace DGR {
	void test3::setup() {
		genWindow(1280, 720, "Danger Renderer");

		initScene();
		
		//IMGUI

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = &ImGui::GetIO(); (void)io;
		io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 450");



		//OGL

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);//default
		glFrontFace(GL_CCW);//default


		mainCamera = CameraManager::Create(glm::vec3(0.0f, 0.0f, 5.0f));
		shader_geom_pass = ShaderManager::Generate("src/shaders/gbuff.vert", "src/shaders/gbuff.frag");
		shader_lighting_pass = ShaderManager::Generate("src/shaders/deferred.vert", "src/shaders/deferred.frag");
		shader_light_box = ShaderManager::Generate("src/shaders/light_box.vert", "src/shaders/light_box.frag");
		shader_portal_pass = ShaderManager::Generate("src/shaders/pink.vert", "src/shaders/pink.frag");
		backpack = ModelManager::Create("src/assets/models/backpack/backpack.obj");
		block = ModelManager::Create("src/assets/models/Box/box.obj");
		panel = ModelManager::Create("src/assets/models/Primitives/quad.obj");
		instance_pos.push_back(glm::vec3(-3.0, -0.5, -3.0));
		/*instance_pos.push_back(glm::vec3(0.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(3.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(-3.0, -0.5, 0.0));*/
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
			float x = 0.5f;
			float y = 1.0f;
			float z = -2.0f;
			float r = 0.5f; 
			float g = 0.5f; 
			float b = 1.0f; 
			const float constant = 14.0;
			const float linear = 0.002;
			const float quadratic = 0.08;
			light_color.x = r;
			light_color.y = g;
			light_color.z = b;
			lights.push_back(
				LightManager::create_volumelight({ r, g, b }, 0.5f, { x, y, z }, constant, linear, quadratic)
			);
		}

		ShaderManager::use(shader_lighting_pass);
		ShaderManager::setUniInt(shader_lighting_pass, "gPosition", 0);
		ShaderManager::setUniInt(shader_lighting_pass, "gNormal", 1);
		ShaderManager::setUniInt(shader_lighting_pass, "gAlbedoSpec", 2);

	}
	void test3::render() {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput();
		if (!freeCursor) {
			io->ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
		else {
			io->ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(mainCamera.zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = CameraManager::GetViewMatrix(mainCamera);
		glm::mat4 model = glm::mat4(1.0f);





		//gbuffer pass
		FramebufferManager::bind(fb);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



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
		/*model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -26.0, 0.0));
		model = glm::scale(model, glm::vec3(25.0f));
		ShaderManager::setUniMat4(shader_geom_pass, "model", model);
		ModelManager::Draw(block, shader_geom_pass);*/





		FramebufferManager::unbind();

		

		// lighting pass
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


		


		//blit gbuffer's depth buffer to default framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);




		// draw boxes at light source
		ShaderManager::use(shader_light_box);
		ShaderManager::setUniMat4(shader_light_box, "projection", projection);
		ShaderManager::setUniMat4(shader_light_box, "view", view);
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, lights[i].position);
			model = glm::scale(model, glm::vec3(0.0625f));
			ShaderManager::setUniMat4(shader_light_box, "model", model);
			ShaderManager::setUniVec3(shader_light_box, "lightColor", lights[i].basic_values.color);
			renderCube();
		}


		

		//Stencil test experement

		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); 
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE); 
		glClear(GL_STENCIL_BUFFER_BIT); 

		ShaderManager::use(shader_portal_pass);
		ShaderManager::setUniMat4(shader_portal_pass, "projection", projection);
		ShaderManager::setUniMat4(shader_portal_pass, "view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 1.0, 0.5));
		model = glm::scale(model, glm::vec3(1, 2, 1));
		ShaderManager::setUniMat4(shader_portal_pass, "model", model);
		ModelManager::Draw(panel, shader_portal_pass);

		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00); 
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);



		ShaderManager::use(shader_portal_pass);
		ShaderManager::setUniMat4(shader_portal_pass, "projection", projection);
		ShaderManager::setUniMat4(shader_portal_pass, "view", view);
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		model = glm::translate(model, glm::vec3(0.0, 0.0, -1.0));
		model = glm::scale(model, glm::vec3(25.0f));
		ShaderManager::setUniMat4(shader_portal_pass, "model", model);
		ModelManager::Draw(panel, shader_portal_pass);

		glDepthMask(GL_TRUE); 
		glDisable(GL_STENCIL_TEST);





		// Render IMGUI windows
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Light values!");

			ImGui::SliderFloat("light x", &lights[0].position.x, -5.0f, 5.0f); 
			ImGui::SliderFloat("light y", &lights[0].position.y, -5.0f, 5.0f); 
			ImGui::SliderFloat("light z", &lights[0].position.z, -7.0f, 3.0f);
			ImGui::ColorEdit3("light color", (float*)&light_color); // Edit 3 floats representing a color
			lights[0].basic_values.color.r = light_color.x;
			lights[0].basic_values.color.g = light_color.y;
			lights[0].basic_values.color.b = light_color.z;

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());






		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	//utility functions

	
	void test3::renderCube()
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

	
	void test3::renderQuad()
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