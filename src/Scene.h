#pragma once
#include "DangerConfig.h"
#include "Danger.h"
#include "util/Logger.h"
#include "util/FileManip.h"
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "structures/structures.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "util/packed_freelist.h"
#include "util/typealias.h"

namespace DGR {

	//opengl error message callback function
	void GLAPIENTRY RNDR_OGL_DBG_MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message, const void* userParam);


	class Scene
	{
	protected:
		//window attributes
		static uint WINDOW_WIDTH;
		static uint WINDOW_HEIGHT;
		Window* window;
		//scene context
		static Camera mainCamera; //viewport camera
		static float mouseX; //horizontal mouse movement
		static float mouseY; //vertical mouse movement
		static bool firstMouse; //if current mouse position is the initial mouse position
		static float deltaTime; //time elapsed since the beginning of the previous frame
		static float lastFrame; 

		static bool freeCursor; //show the cursor and stop constraining it to the window
		static bool tabReleased; //tab button is no longer being pressed

		//setup glfw context
		static void initGLFW() {
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		//create the scene's window
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

		//misc setup for APIs and declaration of variables
		void initScene();

	public:
		//overloadable setup for per-instance stuff
		virtual void setup() {}
		//should the window close?
		bool shouldClose();
		//overloadable render function
		virtual void render() {}
		//destroy scene context and free up memory
		void cleanup();

		//controls what happens when the window is resized
		static void window_resize_callback(GLFWwindow* w, int width, int height)
		{
			WINDOW_WIDTH = width;
			WINDOW_HEIGHT = height;
			glViewport(0, 0, width, height);
		}

		//controls what happens when the mouse moves
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

		//controls what happens when the scroll wheel is used
		static void scroll_callback(GLFWwindow* w, double xoffset, double yoffset)
		{
			CameraManager::UpdateZoom(mainCamera, yoffset);
		}

		//controls what happens when keys are pressed
		void processInput()
		{
			//escape key closes window
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);

			//tab key toggles cursor state.
			if (tabReleased && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
				freeCursor = !freeCursor;
				tabReleased = false;
				glfwSetInputMode(window, GLFW_CURSOR, freeCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
			}
			//need this so that holding down tab doesn't toggle cursor state every frame
			if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
				tabReleased = true;
			}
			//movement keys: WASD, E=up, LSHIFT=down
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

