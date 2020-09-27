#include "test3.h"



/*TODO:
	add dt integration to fix infinite portal teleportation
	make teleport work for arbitrary portal rotation
	make everything work for arbitrary portal rotation
	make near plane clipping work
	abstract portals into an object and object manager
	add a physics system for collision detection.
	maybe create a "world object" / "scene object" abstraction
	make portals recursive

*/

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

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);//default
		glFrontFace(GL_CCW);//default


		



		mainCamera = CameraManager::Create(glm::vec3(0.0f, 0.0f, 5.0f));
		shader_bp = ShaderManager::Generate("src/shaders/models.vert", "src/shaders/models.frag");
		shader_portal_pass = ShaderManager::Generate("src/shaders/pink.vert", "src/shaders/pink.frag");
		backpack = ModelManager::Create("src/assets/models/backpack/backpack.obj");
		block = ModelManager::Create("src/assets/models/Box/box.obj");
		panel = ModelManager::Create("src/assets/models/Primitives/quad.obj");
		instance_pos.push_back(glm::vec3(-3.0, -0.5, -3.0));


		//portals
		p1_pos = glm::vec3(0.0, 1.0, 0.5);
		p2_pos = glm::vec3(-3.0, 0.5, -4.0);
		p1_cam = mainCamera;
		p1_cam.pos = (p2_pos - mainCamera.pos) + p1_pos;
		p1_cam.yaw = 180.0f - mainCamera.yaw;
		CameraManager::UpdateVecs(p1_cam);
		

		p2_cam = mainCamera;
		p2_cam.pos = (p1_pos - mainCamera.pos) + p2_pos;
		p2_cam.yaw = 180.0f - mainCamera.yaw;
		CameraManager::UpdateVecs(p2_cam);
		//p1_cam = glm::inverse(CameraManager::GetViewMatrix(mainCamera));



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

		//ShaderManager::use(shader_lighting_pass);
		//ShaderManager::setUniInt(shader_lighting_pass, "gPosition", 0);
		//ShaderManager::setUniInt(shader_lighting_pass, "gNormal", 1);
		//ShaderManager::setUniInt(shader_lighting_pass, "gAlbedoSpec", 2);

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

		p1_cam.pos = (p2_pos - mainCamera.pos) + p1_pos;
		p1_cam.pos.y = (mainCamera.pos.y - p2_pos.y) + p1_pos.y;
		p1_cam.yaw = 180.0f + mainCamera.yaw;
		p1_cam.pitch = mainCamera.pitch;
		CameraManager::UpdateVecs(p1_cam);
		//if (p1_cam.pos.z < p1_pos.z)
		//	CameraManager::ClipNearPlaneOblique(p1_cam, p1_pos, glm::vec3(0, 0, 1));
		//else
		//	CameraManager::ClipNearPlaneOblique(p1_cam, p1_pos, glm::vec3(0, 0, -1));


		
		p2_cam.pos = (p1_pos - mainCamera.pos) + p2_pos;
		p2_cam.pos.y = (mainCamera.pos.y - p1_pos.y) + p2_pos.y;
		p2_cam.yaw = 180.0f + mainCamera.yaw;
		p2_cam.pitch = mainCamera.pitch;
		CameraManager::UpdateVecs(p2_cam);
		//if (p2_cam.pos.z < p2_pos.z)
		//	CameraManager::ClipNearPlaneOblique(p2_cam, p2_pos, glm::vec3(0, 0, 1));
		//else
		//	CameraManager::ClipNearPlaneOblique(p2_cam, p2_pos, glm::vec3(0, 0, -1));

		glm::mat4 projection = glm::perspective(glm::radians(mainCamera.zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = CameraManager::GetViewMatrix(mainCamera);
		glm::mat4 model = glm::mat4(1.0f);




		



		

		//Stencil test experement portal 1

		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); 
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE); 
		glClear(GL_STENCIL_BUFFER_BIT); 

		ShaderManager::use(shader_bp);
		ShaderManager::setUniMat4(shader_bp, "projection", projection);
		ShaderManager::setUniMat4(shader_bp, "view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, p1_pos);
		model = glm::scale(model, glm::vec3(0.5, 1, 0.5));
		ShaderManager::setUniMat4(shader_bp, "model", model);
		ModelManager::Draw(panel, shader_bp);

		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00); 

		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		ShaderManager::use(shader_bp);
		ShaderManager::setUniMat4(shader_bp, "projection", projection);
		ShaderManager::setUniMat4(shader_bp, "view", CameraManager::GetViewMatrix(p2_cam));
		ShaderManager::setUniVec3(shader_bp, "view_pos", p2_cam.pos);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -26, 0.0));
		model = glm::scale(model, glm::vec3(25.0f));
		ShaderManager::setUniMat4(shader_bp, "model", model);
		ModelManager::Draw(block, shader_bp);

		for (unsigned int i = 0; i < instance_pos.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, instance_pos[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			ShaderManager::setUniMat4(shader_bp, "model", model);
			ShaderManager::setUniVec3(shader_bp, "light_pos", lights[0].position);
			ShaderManager::setUniVec3(shader_bp, "light_col", lights[0].basic_values.color);
			ModelManager::Draw(backpack, shader_bp);
		}
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		ShaderManager::use(shader_bp);
		ShaderManager::setUniMat4(shader_bp, "projection", projection);
		ShaderManager::setUniMat4(shader_bp, "view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, p1_pos);
		model = glm::scale(model, glm::vec3(0.5, 1, 0.5));
		ShaderManager::setUniMat4(shader_bp, "model", model);
		ModelManager::Draw(panel, shader_bp);





		//Stencil test experement portal 2
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		ShaderManager::use(shader_bp);
		ShaderManager::setUniMat4(shader_bp, "projection", projection);
		ShaderManager::setUniMat4(shader_bp, "view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, p2_pos);
		model = glm::scale(model, glm::vec3(0.5, 1, 0.5));
		ShaderManager::setUniMat4(shader_bp, "model", model);
		ModelManager::Draw(panel, shader_bp);

		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);

		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		ShaderManager::use(shader_bp);
		ShaderManager::setUniMat4(shader_bp, "projection", projection);
		ShaderManager::setUniMat4(shader_bp, "view", CameraManager::GetViewMatrix(p1_cam));
		ShaderManager::setUniVec3(shader_bp, "view_pos", p1_cam.pos);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -26, 0.0));
		model = glm::scale(model, glm::vec3(25.0f));
		ShaderManager::setUniMat4(shader_bp, "model", model);
		ModelManager::Draw(block, shader_bp);

		for (unsigned int i = 0; i < instance_pos.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, instance_pos[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			ShaderManager::setUniMat4(shader_bp, "model", model);
			ShaderManager::setUniVec3(shader_bp, "light_pos", lights[0].position);
			ShaderManager::setUniVec3(shader_bp, "light_col", lights[0].basic_values.color);
			ModelManager::Draw(backpack, shader_bp);
		}
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		ShaderManager::use(shader_bp);
		ShaderManager::setUniMat4(shader_bp, "projection", projection);
		ShaderManager::setUniMat4(shader_bp, "view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, p2_pos);
		model = glm::scale(model, glm::vec3(0.5, 1, 0.5));
		ShaderManager::setUniMat4(shader_bp, "model", model);
		ModelManager::Draw(panel, shader_bp);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);









		ShaderManager::use(shader_bp);
		ShaderManager::setUniMat4(shader_bp, "projection", projection);
		ShaderManager::setUniMat4(shader_bp, "view", view);
		ShaderManager::setUniVec3(shader_bp, "view_pos", mainCamera.pos);
		for (unsigned int i = 0; i < instance_pos.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, instance_pos[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			ShaderManager::setUniMat4(shader_bp, "model", model);
			ShaderManager::setUniVec3(shader_bp, "light_pos", lights[0].position);
			ShaderManager::setUniVec3(shader_bp, "light_col", lights[0].basic_values.color);
			ModelManager::Draw(backpack, shader_bp);
		}
		//using a box for the ground
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -26.001, 0.0));
		model = glm::scale(model, glm::vec3(25.0f));
		ShaderManager::setUniMat4(shader_bp, "model", model);
		ModelManager::Draw(block, shader_bp);

		//ShaderManager::use(shader_portal_pass);
		//ShaderManager::setUniMat4(shader_portal_pass, "projection", projection);
		//ShaderManager::setUniMat4(shader_portal_pass, "view", view);
		//model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		//model = glm::translate(model, glm::vec3(0.0, 0.0, -1.01));
		//model = glm::scale(model, glm::vec3(25.0f));
		//ShaderManager::setUniMat4(shader_portal_pass, "model", model);
		//ModelManager::Draw(panel, shader_portal_pass);


		if (abs(mainCamera.pos.z - p1_pos.z) < 0.05f) {
			float xoffs = mainCamera.pos.x - p1_pos.x;
			float yoffs = mainCamera.pos.y - p1_pos.y;
			if (abs(xoffs) < 0.5 && abs(yoffs) < 1.0) {
				mainCamera.pos = p2_pos;
				mainCamera.pos.x += xoffs;
				mainCamera.pos.y += yoffs;
				mainCamera.yaw = p2_cam.yaw;
				CameraManager::UpdateVecs(mainCamera);
				CameraManager::UpdatePosition(mainCamera, Movement::FORWARD, deltaTime);
			}
		}
		if (abs(mainCamera.pos.z - p2_pos.z) < 0.15f) {
			float xoffs = mainCamera.pos.x - p2_pos.x;
			float yoffs = mainCamera.pos.y - p2_pos.y;
			if (abs(xoffs) < 0.5 && abs(yoffs) < 1.0) {
				mainCamera.pos = p1_pos;
				mainCamera.pos.x += xoffs;
				mainCamera.pos.y += yoffs;
				mainCamera.yaw = p1_cam.yaw;
				CameraManager::UpdateVecs(mainCamera);
				CameraManager::UpdatePosition(mainCamera, Movement::FORWARD, deltaTime);
			}
		}



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
			ImGui::SliderFloat("portal cam x", &p2_pos.x, -10.0f, 10.0f);
			ImGui::SliderFloat("portal cam y", &p2_pos.y, -10.0f, 10.0f);
			ImGui::SliderFloat("portal cam z", &p2_pos.z, -10.0f, 10.0f);

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