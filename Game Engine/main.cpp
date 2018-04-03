#pragma warning
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include "glfw\include\GLFW\glfw3.h"
#include "ImGui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"
#include <stdio.h>

// test 

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
#include "SSAOShaders.h"
#include "SSAOBlurShaders.h"
#include "LightpassShaders.h"
#include "FXAAShaders.h"
#include "CubeMapShaders.h"
#define _CRTDBG_MAP_ALLOC
////////////
//Render

static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

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
		GLFWwindow* window = glfwCreateWindow(1280, 720, "Game Engine", NULL, NULL);
		glfwMakeContextCurrent(window);
		//glfwSwapInterval(1); // Enable vsync
		gl3wInit();

		// Setup ImGui binding
		ImGui_ImplGlfwGL3_Init(window, true);

		// Setup style
		ImGui::StyleColorsClassic();
		//ImGui::StyleColorsDark();

		bool show_demo_window = false;
		bool show_hierarchy_window = true;
		bool show_inspector_window = true;
		bool show_another_window = false;
		int gaussianblur = 0;
		int fxaa = 0;
		int ssao = 1;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		////////////////
		float oldTime = float(clock());
		float elapsedTime = 0;

		GameScene gameScene;
		ShaderProgramLib shaderProgramLibrary;
		//shader måste skapas innan mesh och material
		shaderProgramLibrary.addGeometryPassShaders();
		shaderProgramLibrary.addCubeMapShaders();
		shaderProgramLibrary.addSSAOShaders();
		shaderProgramLibrary.addBlurShaders();
		shaderProgramLibrary.addLightpassShaders();
		shaderProgramLibrary.addSkyboxShaders();
		shaderProgramLibrary.addGaussianBlurShaders();
		shaderProgramLibrary.addFXAAShaders();

		RenderManager renderManager = RenderManager( &gameScene, window, &shaderProgramLibrary );

		MaterialLib materialLibrary;
		TextureLib textureLibrary;
		MeshLib meshLibrary;


		//... Create Camera and add empty game object
		CharacterMovement moveScript = CharacterMovement(window);
		gameScene.addEmptyGameObject();

		//... Create Lights and add empty game object
		Light light1 = Light();
		Light light2 = Light();
		Light light3 = Light();
		Light light4 = Light();
		Light light5 = Light();
		gameScene.addEmptyGameObject();
		gameScene.addEmptyGameObject();
		gameScene.addEmptyGameObject();
		gameScene.addEmptyGameObject();
		gameScene.addEmptyGameObject();

		int nrOfObjects = gameScene.gameObjects.size();

		//... Add name of OBJ to add to scene
		std::string meshName[] = { "Floor.obj" };

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
		gameScene.gameObjects[1].transform = glm::vec3(-1, 0.4, 3);

		gameScene.gameObjects[2].name = "Light 2";
		gameScene.gameObjects[2].addComponent(&light2);
		gameScene.gameObjects[2].transform = glm::vec3(4, 0.4, -2);

		gameScene.gameObjects[3].name = "Light 3";
		gameScene.gameObjects[3].addComponent(&light3);
		gameScene.gameObjects[3].transform = glm::vec3(1, 0.4, -3);

		gameScene.gameObjects[4].name = "Light 4";
		gameScene.gameObjects[4].addComponent(&light4);
		gameScene.gameObjects[4].transform = glm::vec3(-5, 0.4, -4);

		gameScene.gameObjects[5].name = "Light 5";
		gameScene.gameObjects[5].addComponent(&light5);
		gameScene.gameObjects[5].transform = glm::vec3(-5, 0.4, 3);

		MeshFilter meshFilter[sizeof(meshName) / sizeof(meshName[0])];

		for (int i = 0; i < nrOfMeshes; i++)
		{
			meshFilter[i] = MeshFilter(meshLibrary.getMesh(i).gVertexBuffer, meshLibrary.getMesh(i).gVertexAttribute, meshLibrary.getMesh(i).gElementBuffer, meshLibrary.getMesh(i).vertexCount);
			gameScene.gameObjects[i + 6].name = meshName[i];
			gameScene.gameObjects[i + 6].addComponent(&meshFilter[i]);
			gameScene.gameObjects[i + 6].addComponent(materialLibrary.getMaterial(i));
		}

		//... Uniform in int that tells gaussian to be turned off
		glUseProgram(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram, "onOrOff"), gaussianblur);

		glUseProgram(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram, "swap"), fxaa);

		glEnable(GL_DEPTH_TEST);

		glDepthFunc(GL_LESS);

		//////////////
		int gameObject_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.

													// Main loop
		while (!glfwWindowShouldClose(window))
		{

			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			// 1. Show a simple window.
			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			static float f = 0.0f;
			{
				ImGui::Text("Hello, world!");                           // Some text (you can use a format string too)
				ImGui::SliderFloat("float", &f, -1.0f, 1.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
				if (ImGui::Button("ImGui Demo"))
					show_demo_window ^= 1;
				if (ImGui::Button("Game Object Window"))
					show_hierarchy_window ^= 1;
				if (ImGui::Button("Blur Scene"))
				{
					if (gaussianblur == 0)
						gaussianblur = 1;
					else
						gaussianblur = 0;
					glUseProgram(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram);
					glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram, "onOrOff"), gaussianblur);
				}
				if (ImGui::Button("FXAA"))
				{
					if (fxaa == 0)
						fxaa = 1;
					else
						fxaa = 0;

					glUseProgram(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram);
					glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram, "swap"), fxaa);
				}
				if (ImGui::Button("SSAO"))
				{
					if (ssao == 0)
						ssao = 1;
					else
						ssao = 0;
				}

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			if (show_demo_window)
			{
				ImGui::ShowTestWindow();
			}

			//hierarchy window
			if (show_hierarchy_window)
			{
				ImGui::Begin("Game Objects", &show_hierarchy_window);

				static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
				ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
				for (unsigned int i = 0; i < gameScene.gameObjects.size(); i++)
				{
					// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);

					{
						// Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
						node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
						string gameObjectName = gameScene.gameObjects[i].name;
						const char * c = gameObjectName.c_str();

						ImGui::TreeNodeEx(c, node_flags, "%s", c);
						if (ImGui::IsItemClicked())
							gameObject_clicked = i;
					}
				}
				if (gameObject_clicked != -1)
				{
					// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
					if (ImGui::GetIO().KeyCtrl)
						selection_mask ^= (1 << gameObject_clicked);          // CTRL+click to toggle
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
						selection_mask = (1 << gameObject_clicked);           // Click to single-select
				}
				ImGui::PopStyleVar();


				ImGui::End();
			}
			//std::cout << (gameObject_clicked) << std::endl;

			//Inspector window
			if (show_inspector_window && gameObject_clicked != -1)
			{
				ImGui::Begin("Inspector", &show_inspector_window);
				ImGui::Text("Inspector");

				static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
				int component_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
				ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.


																							//start transform
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << 0)) ? ImGuiTreeNodeFlags_Selected : 0);

				// Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
				node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << 0)) ? ImGuiTreeNodeFlags_Selected : 0);
				string componentName = "Transform";

				const char * c = componentName.c_str();

				bool node_open = ImGui::TreeNodeEx(c, node_flags, "%s", c);
				if (ImGui::IsItemClicked())
					component_clicked = 0;
				if (node_open)
				{
					ImGui::SliderFloat("x", &gameScene.gameObjects[gameObject_clicked].transform.position.x, -10.0f, 10.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
					ImGui::SliderFloat("y", &gameScene.gameObjects[gameObject_clicked].transform.position.y, -10.0f, 10.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
					ImGui::SliderFloat("z", &gameScene.gameObjects[gameObject_clicked].transform.position.z, -10.0f, 10.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
					ImGui::Text("Empty object is empty");
					ImGui::TreePop();
				}
				//end transform
				if (ImGui::IsItemClicked())
					component_clicked = 0;

				static bool options_menu = true;
				static bool alpha_preview = true;
				static bool alpha_half_preview = false;
				static bool hdr = false;
				static ImVec4 color = ImColor(114, 144, 154, 200);

				int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

				for (unsigned int i = 0; i < gameScene.gameObjects[gameObject_clicked].components.size(); i++)
				{
					// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);

					// Node
					string componentName = gameScene.gameObjects[gameObject_clicked].components[i]->assetName;
					const char * c = componentName.c_str();

					node_open = ImGui::TreeNodeEx(c, node_flags, "%s", c);
					if (ImGui::IsItemClicked())
						component_clicked = i;
					if (node_open)
					{
						Light* lightGUI = gameScene.gameObjects[gameObject_clicked].lightComponent;
						if (lightGUI != nullptr) {
							ImGui::Text("Color button with Picker:");
							ImGui::ColorEdit4("Color##1", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
							glm::vec4 tempColor = glm::vec4(color.x, color.y, color.z, color.w);
							lightGUI->color = tempColor;
						}
						ImGui::Text("Add Stuff Here");
						ImGui::TreePop();
					}


				}
				if (component_clicked != -1)
				{
					// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
					if (ImGui::GetIO().KeyCtrl)
						selection_mask ^= (1 << component_clicked);          // CTRL+click to toggle
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
						selection_mask = (1 << component_clicked);           // Click to single-select
				}
				ImGui::PopStyleVar();


				ImGui::End();
			}

			//Create delta time
			float currentTime = float(clock());
			elapsedTime -= (currentTime - oldTime)*0.05f*f;
			oldTime = currentTime;

			for (unsigned int b = 0; b < gameScene.gameObjects.size(); b++)
			{
				for (unsigned int i = 0; i < gameScene.gameObjects[b].components.size(); i++)
				{
					gameScene.gameObjects[b].components[i]->update();
				}
			}

			renderManager.Render(elapsedTime, ssao);

			ImGui::Render();
			glfwSwapBuffers(window);
		}

		ImGui_ImplGlfwGL3_Shutdown();
		glfwTerminate();
	}
	_CrtDumpMemoryLeaks();

	return 0;
}
