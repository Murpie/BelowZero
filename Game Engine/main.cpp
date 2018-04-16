#pragma warning
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include "glfw\include\GLFW\glfw3.h"
#include <stdio.h>
#include <time.h>

/////////////
#include "MaterialLib.h"
#include "TextureLib.h"
#include "ShaderProgramLib.h"
#include "MeshLib.h"
#include "GameScene.h"
#include "RenderManager.h"
#include "Transform.h"
#include "MeshFilter.h"
#include "CharacterMovement.h"
#include <string>
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "glm\glm\glm.hpp"
#include "Light.h"
#include <ctime>
#include "GeometryShaders.h"
#include "LightpassShaders.h"
#include "CubeMapShaders.h"

#include <chrono>
#include "PointLightShadowMapShaders.h"
#define _CRTDBG_MAP_ALLOC
////////////
//Render
auto startSeconds = chrono::high_resolution_clock::now();
auto startDeltaTime = chrono::high_resolution_clock::now();
auto test = chrono::high_resolution_clock::now();


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**)
{
	{
		// Setup window
		glfwSetErrorCallback(error_callback);
		if (!glfwInit())
			return 1;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		int count;
		GLFWmonitor** primary = glfwGetMonitors(&count);
		const GLFWvidmode* mode = glfwGetVideoMode(primary[0]);
		GLFWwindow* window = glfwCreateWindow(1280, 720, "Game Engine", NULL, NULL);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(0);
		gl3wInit();

		bool fullscreen = false;

		int initial_time = time(NULL);
		int final_time;
		int frameCount = 0;

		GameScene gameScene;
		ShaderProgramLib shaderProgramLibrary;
		//shader måste skapas innan mesh och material
		shaderProgramLibrary.addGeometryPassShaders();
		shaderProgramLibrary.addCubeMapShaders();
		shaderProgramLibrary.addLightpassShaders();
		shaderProgramLibrary.addSkyboxShaders();
		shaderProgramLibrary.addShadowMapShaders();
		shaderProgramLibrary.addPointLightShadowMapShaders();
		//shaderProgramLibrary.addAnimationShaders();
		shaderProgramLibrary.addUIShaders();

		RenderManager renderManager = RenderManager(&gameScene, window, &shaderProgramLibrary);

		MaterialLib materialLibrary;
		TextureLib textureLibrary;
		MeshLib meshLibrary;


		
		//... Create Camera and add empty game object
		CharacterMovement moveScript = CharacterMovement(window);
		gameScene.addEmptyGameObject();

		//... Create Lights and add empty game object
		Light light1 = Light();
		Light light2 = Light();
		/*Light light3 = Light();
		Light light4 = Light();
		Light light5 = Light();*/
		gameScene.addEmptyGameObject();
		gameScene.addEmptyGameObject();
		/*gameScene.addEmptyGameObject();
		gameScene.addEmptyGameObject();
		gameScene.addEmptyGameObject();*/

		int nrOfObjects = gameScene.gameObjects.size();

		//... Add name of OBJ to add to scene
		std::string meshName[] = { "Floor.obj", "Tree.obj", "Bucket.obj", "TreeWithSnow.obj", "Stump.obj", "Stone.obj" };

		int nrOfMeshes = sizeof(meshName) / sizeof(meshName[0]);

		//... Read OBJ and MTL File
		for (int i = 0; i < nrOfMeshes; i++)
		{
			gameScene.addEmptyGameObject();
			meshLibrary.addMesh(meshName[i], shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram);
			//Add material
			materialLibrary.addMaterial(shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram);

			//If found variables are not querried, Set standard values in shader.
			//1: send in texture
			//2: send in mtl values
			unsigned int foundAlbedo = 0;
			unsigned int foundNormal = 0;
			unsigned int foundSpecular = 0;
			unsigned int foundMetallic = 0;
			unsigned int foundAO = 0;
			glm::mat3 mtlInfo;

			const char *meshNameChar = meshName[i].c_str();
			FILE *file = fopen(meshNameChar, "r");
			while (file != NULL)
			{
				char lineHeader[128];
				int word = fscanf(file, "%s", lineHeader);
				if (word == EOF) //EOF = End Of Line.
				{
					materialLibrary.getMaterial(i)->addFoundAlbedo(foundAlbedo);
					materialLibrary.getMaterial(i)->addFoundNormal(foundNormal);
					materialLibrary.getMaterial(i)->addFoundSpecular(foundSpecular);
					materialLibrary.getMaterial(i)->addFoundMetallic(foundMetallic);
					materialLibrary.getMaterial(i)->addFoundAO(foundAO);
					materialLibrary.getMaterial(i)->addMtlInfo(mtlInfo);

					foundAlbedo = 0;
					foundSpecular = 0;
					foundNormal = 0;
					foundMetallic = 0;
					foundAO = 0;
					break;
				}
				else if (strcmp(lineHeader, "mtllib") == 0)
				{

					char mtlChar[128];
					fscanf(file, "%s", mtlChar);
					FILE* mtlFile = fopen(mtlChar, "r");

					while (true)
					{
						int textureLocation = 0;
						char mtlLineHeader[128];
						int mtlWord = fscanf(mtlFile, "%s", mtlLineHeader);
						if (mtlWord == EOF)
						{
							break;
						}
						else if (strcmp(mtlLineHeader, "Kd") == 0)
						{
							fscanf(mtlFile, "%f %f %f", &mtlInfo[0].x, &mtlInfo[0].y, &mtlInfo[0].z);
							foundAlbedo = 2;
						}
						else if (strcmp(mtlLineHeader, "Ks") == 0)
						{
							fscanf(mtlFile, "%f %f %f", &mtlInfo[1].x, &mtlInfo[1].y, &mtlInfo[1].z);
							foundSpecular = 2;
						}
						else if (strcmp(mtlLineHeader, "map_Kd") == 0)
						{
							char textureFile[128];
							fscanf(mtlFile, "%s", textureFile);
							textureLibrary.addAlbedo(textureFile);
							if (textureLibrary.albedo.size() - 1 != i)
							{
								for (int j = i; j > 0; j--)
								{
									if (j == textureLibrary.albedo.size() - 1)
									{
										textureLocation = j;
										break;
									}
								}
							}
							else
							{
								textureLocation = i;
							}
							materialLibrary.getMaterial(i)->addAlbedo(textureLibrary.getAlbedo(textureLocation)->gTexture);
							foundAlbedo = 1;
						}
						else if (strcmp(mtlLineHeader, "map_Ks") == 0)
						{
							char textureFile[128];
							fscanf(mtlFile, "%s", textureFile);
							textureLibrary.addSpecular(textureFile);
							if (textureLibrary.specular.size() - 1 != i)
							{
								for (int j = i; j > 0; j--)
								{
									if (j == textureLibrary.specular.size() - 1)
									{
										textureLocation = j;
										break;
									}
								}
							}
							else
							{
								textureLocation = i;
							}
							materialLibrary.getMaterial(i)->addSpecular(textureLibrary.getSpecular(textureLocation)->gTexture);
							foundSpecular = 1;
						}
						else if (strcmp(mtlLineHeader, "map_Normal") == 0)
						{
							char textureFile[128];
							fscanf(mtlFile, "%s", textureFile);
							textureLibrary.addNormal(textureFile);
							if (textureLibrary.normal.size() - 1 != i)
							{
								for (int j = i; j > 0; j--)
								{
									if (j == textureLibrary.normal.size() - 1)
									{
										textureLocation = j;
										break;
									}
								}
							}
							else
							{
								textureLocation = i;
							}
							materialLibrary.getMaterial(i)->addNormal(textureLibrary.getNormal(textureLocation)->gTexture);
							foundNormal = 1;
						}
						else if (strcmp(mtlLineHeader, "map_Metallic") == 0)
						{
							char textureFile[128];
							fscanf(mtlFile, "%s", textureFile);
							textureLibrary.addMetallic(textureFile);
							if (textureLibrary.metallic.size() - 1 != i)
							{
								for (int j = i; j > 0; j--)
								{
									if (j == textureLibrary.metallic.size() - 1)
									{
										textureLocation = j;
										break;
									}
								}
							}
							else
							{
								textureLocation = i;
							}
							materialLibrary.getMaterial(i)->addMetallic(textureLibrary.getMetallic(textureLocation)->gTexture);
							foundMetallic = 1;
						}
						else if (strcmp(mtlLineHeader, "map_AO") == 0)
						{
							char textureFile[128];
							fscanf(mtlFile, "%s", textureFile);
							textureLibrary.addAO(textureFile);
							if (textureLibrary.ao.size() - 1 != i)
							{
								for (int j = i; j > 0; j--)
								{
									if (j == textureLibrary.ao.size() - 1)
									{
										textureLocation = j;
										break;
									}
								}
							}
							else
							{
								textureLocation = i;
							}
							materialLibrary.getMaterial(i)->addAO(textureLibrary.getAO(textureLocation)->gTexture);
							foundAO = 1;
						}
					}
				}
			}
		}

		//... Set Game Objects
		gameScene.gameObjects[0].name = "Camera";
		gameScene.gameObjects[0].addComponent(&moveScript);

		gameScene.gameObjects[1].name = "Light 1";
		gameScene.gameObjects[1].addComponent(&light1);
		gameScene.gameObjects[1].transform = glm::vec3(-4, 8, 0);
		gameScene.gameObjects[1].lightComponent->lightType = 0;

		gameScene.gameObjects[2].name = "Light 2";
		gameScene.gameObjects[2].addComponent(&light2);
		gameScene.gameObjects[2].transform = glm::vec3(4, 0.4, -2);
		gameScene.gameObjects[2].lightComponent->lightType = 1;

		/*gameScene.gameObjects[3].name = "Light 3";
		gameScene.gameObjects[3].addComponent(&light3);
		gameScene.gameObjects[3].transform = glm::vec3(1, 0.4, -3);
		gameScene.gameObjects[3].lightComponent->lightType = 1;

		gameScene.gameObjects[4].name = "Light 4";
		gameScene.gameObjects[4].addComponent(&light4);
		gameScene.gameObjects[4].transform = glm::vec3(-5, 0.4, -4);
		gameScene.gameObjects[4].lightComponent->lightType = 1;

		gameScene.gameObjects[5].name = "Light 5";
		gameScene.gameObjects[5].addComponent(&light5);
		gameScene.gameObjects[5].transform = glm::vec3(-5, 0.4, 3);
		gameScene.gameObjects[5].lightComponent->lightType = 1;*/

		MeshFilter meshFilter[sizeof(meshName) / sizeof(meshName[0])];

		for (int i = 0; i < nrOfMeshes; i++)
		{
			meshFilter[i] = MeshFilter(meshLibrary.getMesh(i).gVertexBuffer, meshLibrary.getMesh(i).gVertexAttribute, meshLibrary.getMesh(i).gElementBuffer, meshLibrary.getMesh(i).vertexCount);
			gameScene.gameObjects[i + 3].name = meshName[i];
			gameScene.gameObjects[i + 3].addComponent(&meshFilter[i]);
			gameScene.gameObjects[i + 3].addComponent(materialLibrary.getMaterial(i));
		}

		glEnable(GL_DEPTH_TEST);

		glDepthFunc(GL_LESS);

		//////////////
		int gameObject_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
		/*glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);*/
													// Main loop
		while (!glfwWindowShouldClose(window))
		{
			if (glfwGetKey(window, GLFW_KEY_F5) && !fullscreen)
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
				glfwSetWindowShouldClose(window, GL_TRUE);

			
			float deltaTime;
			auto nowDeltaTime = chrono::high_resolution_clock::now();
			deltaTime = chrono::duration_cast<chrono::duration<float>>(nowDeltaTime - test).count() * 0.1;
			test = nowDeltaTime;

			
			float secondsTime;
			auto nowSeconds = chrono::high_resolution_clock::now();
			float seconds = (float)chrono::duration_cast<std::chrono::milliseconds>(nowSeconds - startSeconds).count();
			nowSeconds = startSeconds;

			glfwPollEvents();

			for (unsigned int b = 0; b < gameScene.gameObjects.size(); b++)
			{
				for (unsigned int i = 0; i < gameScene.gameObjects[b].components.size(); i++)
				{
					gameScene.gameObjects[b].components[i]->update();
				}
			}


			renderManager.getDeltaTime(deltaTime);
			renderManager.getSeconds(seconds);
			renderManager.Render();

			glfwSwapBuffers(window);

			frameCount++;
			final_time = time(NULL);
			if (final_time - initial_time > 0)
			{
				string temp = "Game Engine FPS : " + std::to_string(frameCount / (final_time - initial_time));
				char* windowName = const_cast<char*>(temp.c_str());
				glfwSetWindowTitle(window, windowName);
				frameCount = 0;
				initial_time = final_time;
			}
		}
		glfwTerminate();
	}
	_CrtDumpMemoryLeaks();

	return 0;
}