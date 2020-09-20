#pragma once
#include "util/Logger.h"
#include "util/FileManip.h"
#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "structures/structures.h"

#include <iostream>

#include "Danger.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "util/packed_freelist.h"
#include "structures/types.h"

namespace DGR {

	void GLAPIENTRY RNDR_OGL_DBG_MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message, const void* userParam);


	class Scene
	{
	protected:
		static uint WINDOW_WIDTH;
		static uint WINDOW_HEIGHT;
		Window* window;
		static Camera mainCamera;
		static float mouseX;
		static float mouseY;
		static bool firstMouse;
		static float deltaTime;
		static float lastFrame;
		static bool freeCursor, tabReleased;
		static void initGLFW() {
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}
		void genWindow(uint width, uint height, const char* name) {

			initGLFW();
			WINDOW_WIDTH = width;
			WINDOW_HEIGHT = height;
			freeCursor = false;
			tabReleased = true;

			GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, name, NULL, NULL);
			if (window == NULL)
			{
				glfwTerminate();
				DGR_ASSERT(false, "Failed to create GLFW window");
			}
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, window_resize_callback);
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetScrollCallback(window, scroll_callback);


			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			this->window = window;
		}
		void initScene();

	public:
		virtual void setup() {}
		bool shouldClose();
		virtual void render() {}
		void cleanup();

		static void window_resize_callback(GLFWwindow* w, int width, int height)
		{
			WINDOW_WIDTH = width;
			WINDOW_HEIGHT = height;
			glViewport(0, 0, width, height);
		}


		static void mouse_callback(GLFWwindow* w, double xpos, double ypos)
		{
			if (!freeCursor) {
				if (firstMouse)
				{
					mouseX = xpos;
					mouseY = ypos;
					firstMouse = false;
				}

				float xoffset = xpos - mouseX;
				float yoffset = mouseY - ypos; // reversed since y-coordinates go from bottom to top

				mouseX = xpos;
				mouseY = ypos;

				CameraManager::UpdateEulerAngles(mainCamera, xoffset, yoffset);
			}
		}


		static void scroll_callback(GLFWwindow* w, double xoffset, double yoffset)
		{
			CameraManager::UpdateZoom(mainCamera, yoffset);
		}
		void processInput()
		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);
			if (tabReleased && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
				freeCursor = !freeCursor;
				tabReleased = false;
				glfwSetInputMode(window, GLFW_CURSOR, freeCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
			}
			if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
				tabReleased = true;
			}

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				CameraManager::UpdatePosition(mainCamera, FORWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				CameraManager::UpdatePosition(mainCamera, BACKWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				CameraManager::UpdatePosition(mainCamera, LEFT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				CameraManager::UpdatePosition(mainCamera, RIGHT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				CameraManager::UpdatePosition(mainCamera, UP, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				CameraManager::UpdatePosition(mainCamera, DOWN, deltaTime);
		}
	};
}

