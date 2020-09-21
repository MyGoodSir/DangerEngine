#include "test2.h"

namespace DGR {
	void test2::setup() {
		genWindow(800, 600, "Danger Renderer");

		initScene();

		mainCamera = CameraManager::Create(glm::vec3(0.0f, 0.0f, 5.0f));
		//create shaders
		gbuff_pass = ShaderManager::Generate("src/shaders/gbuff.vert", "src/shaders/gbuff.frag");
		lighting_pass = ShaderManager::Generate("src/shaders/deferred.vert", "src/shaders/deferred.frag");
		light_geometry = ShaderManager::Generate("src/shaders/light_box.vert", "src/shaders/light_box.frag");
		//create models
		backpack = ModelManager::Create("src/assets/models/backpack/backpack.obj");
		block = ModelManager::Create("src/assets/models/Box/box.obj");
		//set instance positions
		instance_pos.push_back(glm::vec3(-3.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(0.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(3.0, -0.5, -3.0));
		instance_pos.push_back(glm::vec3(-3.0, -0.5, 0.0));
		instance_pos.push_back(glm::vec3(0.0, -0.5, 0.0));
		instance_pos.push_back(glm::vec3(3.0, -0.5, 0.0));
		instance_pos.push_back(glm::vec3(-3.0, -0.5, 3.0));
		instance_pos.push_back(glm::vec3(0.0, -0.5, 3.0));
		instance_pos.push_back(glm::vec3(3.0, -0.5, 3.0));
		//create gbuffer
		fb = FramebufferManager::create(WINDOW_WIDTH, WINDOW_HEIGHT);
		//create gbuffer textures
		FBTexture gPos{ GL_RGBA, GL_RGBA16F, GL_FLOAT };
		FBTexture gNorm{ GL_RGBA, GL_RGBA16F, GL_FLOAT };
		FBTexture gAlbSpec{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE };
		FramebufferManager::create_fbtexture(fb, gPos);
		FramebufferManager::create_fbtexture(fb, gNorm);
		FramebufferManager::create_fbtexture(fb, gAlbSpec);
		FramebufferManager::draw_target_all(fb);
		//create grenderbuffer
		FramebufferManager::gen_renderbuffer(fb);

		//create lights with random values
		srand(13);
		for (unsigned int i = 0; i < NUM_LIGHTS; i++)
		{
			//light position
			float x = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float y = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			float z = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			//light color
			float r = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float g = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float b = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			//light attenuation
			const float constant = 1.0;
			const float linear = 0.7;
			const float quadratic = 1.8;
			lights.push_back(
				LightManager::create_volumelight({ r, g, b }, 0, { x, y, z }, constant, linear, quadratic)
			);
		}
		//set texture uniform locations for lighting shader
		ShaderManager::use(lighting_pass);
		ShaderManager::setUniInt(lighting_pass, "gPosition", 0);
		ShaderManager::setUniInt(lighting_pass, "gNormal", 1);
		ShaderManager::setUniInt(lighting_pass, "gAlbedoSpec", 2);

	}
	void test2::render() {

		//time calculation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//check keypresses
		processInput();

		//reset the default framebuffer's color and depth buffers
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		
		/*\
		 *
		 *	first pass: gbuffer
		 *
		\*/

		
		FramebufferManager::bind(fb);//bind secondary framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear secondary framebuffer
		//set up transformation matricies
		glm::mat4 projection = glm::perspective(glm::radians(mainCamera.zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = CameraManager::GetViewMatrix(mainCamera);
		glm::mat4 model = glm::mat4(1.0f);
		//use gbuff shader
		ShaderManager::use(gbuff_pass);
		//set projection and view matrix uniforms
		ShaderManager::setUniMat4(gbuff_pass, "projection", projection);
		ShaderManager::setUniMat4(gbuff_pass, "view", view);
		//draw model instances to framebuffer textures
		for (unsigned int i = 0; i < instance_pos.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, instance_pos[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			ShaderManager::setUniMat4(gbuff_pass, "model", model);//set individual model matrix per instance
			ModelManager::Draw(backpack, gbuff_pass);
		}
		//using a box for the ground
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -26.0, 0.0));
		model = glm::scale(model, glm::vec3(25.0f));
		ShaderManager::setUniMat4(gbuff_pass, "model", model);
		ModelManager::Draw(block, gbuff_pass);
		FramebufferManager::unbind();//unbind secondary framebuffer
		//end of gbuff pass

		/*\
		 *
		 *	second pass: lighting
		 *
		\*/

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear default framebuffer
		//use lighting shader
		ShaderManager::use(lighting_pass);
		//bind the 3 color attachments of the secondary framebuffer to texture uniform positions 0, 1, and 2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fb.attachments[0].handle);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fb.attachments[1].handle);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, fb.attachments[2].handle);

		// pass light data into shader uniform array of structures
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			ShaderManager::setUniVec3(lighting_pass, "lights[" + std::to_string(i) + "].Position", lights[i].position);
			ShaderManager::setUniVec3(lighting_pass, "lights[" + std::to_string(i) + "].Color", lights[i].basic_values.color);
			ShaderManager::setUniFloat(lighting_pass, "lights[" + std::to_string(i) + "].Linear", lights[i].attenuation.linear);
			ShaderManager::setUniFloat(lighting_pass, "lights[" + std::to_string(i) + "].Quadratic", lights[i].attenuation.quadratic);
			ShaderManager::setUniFloat(lighting_pass, "lights[" + std::to_string(i) + "].Radius", lights[i].radius);
		}
		ShaderManager::setUniVec3(lighting_pass, "viewPos", mainCamera.pos);//set view position uniform
		
		//lighting pass renders to a quad that covers the whole frame.
		//it uses the 2 textures created by the gbuffer pass to determine material properties, 
		//then determines color by using the corresponding pixel in the secondary framebuffer's default color attachment 
		//and applying lighting using the uniform array we passed to it and the material properties it got from the other textures,
		//then it draws the color to the secondary framebuffer's default color attachment
		renderQuad();
		//end of lighting pass


		/*\
		 *	copy the contents of the secondary framebuffer's default color attachment to the default framebuffer's default color attachment
		\*/
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*\
		 *
		 *	third pass: generate cube geometry for the light objects
		 *
		\*/
		ShaderManager::use(light_geometry);
		ShaderManager::setUniMat4(light_geometry, "projection", projection);
		ShaderManager::setUniMat4(light_geometry, "view", view);
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, lights[i].position);
			model = glm::scale(model, glm::vec3(0.125f));
			ShaderManager::setUniMat4(light_geometry, "model", model);
			ShaderManager::setUniVec3(light_geometry, "lightColor", lights[i].basic_values.color);
			renderCube();
		}




		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	void test2::renderCube()
	{
		
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
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}



	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void test2::renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texcoords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			
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