#pragma warning


	/*	int count;
		GLFWmonitor** primary = glfwGetMonitors(&count);
		const GLFWvidmode* mode = glfwGetVideoMode(primary[0]);
		GLFWwindow* window = glfwCreateWindow(1280, 720, "Game Engine", NULL, NULL);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(0);
		gl3wInit();*/

		//bool fullscreen = false;

		shaderProgramLibrary.addUIShaders();

	
		//std::string meshName[] = { "Floor.obj", "Tree.obj", "Bucket.obj", "TreeWithSnow.obj", "Stump.obj", "Stone.obj" };


			/*if (glfwGetKey(window, GLFW_KEY_F5) && !fullscreen)
			{
				glfwSetWindowMonitor(window, primary[0], 0, 0, 1280, 720, mode->refreshRate);
				fullscreen = true;
			}
			else if (glfwGetKey(window, GLFW_KEY_F5) && fullscreen)
			{
				glfwSetWindowMonitor(window, 0, 100, 100, 1280, 720, mode->refreshRate);
				fullscreen = false;
			}

			if (glfwGetKey(window, GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(window, GL_TRUE);*/



#include "Game.h"

#define _CRTDBG_MAP_ALLOC

int main()
{
	Game game;
	game.run();
	
	_CrtDumpMemoryLeaks();

	return 0;
}
