#include "Scene.h"

namespace DGR {

	Camera Scene::mainCamera;
	float Scene::mouseX;
	float Scene::mouseY;
	bool Scene::firstMouse;
	float Scene::deltaTime;
	float Scene::lastFrame;
	uint Scene::WINDOW_WIDTH;
	uint Scene::WINDOW_HEIGHT;
	bool Scene::freeCursor;
	bool Scene::tabReleased;
	void Scene::initScene() {
		mainCamera = {};
		mouseX = WINDOW_WIDTH / 2.0f;
		mouseY = WINDOW_HEIGHT / 2.0f;
		firstMouse = true;
		deltaTime = 0.0f;
		lastFrame = 0.0f;
		InitManagers();

		DGR_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(RNDR_OGL_DBG_MessageCallback, 0);
		glEnable(GL_DEPTH_TEST);

		stbi_set_flip_vertically_on_load(true);
	}

	bool Scene::shouldClose() {
		return glfwWindowShouldClose(window);
	}

	void Scene::cleanup() {
		glfwTerminate();
	}



	void GLAPIENTRY DGR::RNDR_OGL_DBG_MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message, const void* userParam)
	{
		if (type == GL_DEBUG_TYPE_ERROR)
		{
			DGR_LOG_ERROR("GL CALLBACK: type = ", type, ", severity = ", severity, ", message = ", message, "\n");
		}
		else
		{
			DGR_LOG_INFO("GL CALLBACK: type = ", type, ", severity = ", severity, ", message = ", message, "\n");
		}
	}
}