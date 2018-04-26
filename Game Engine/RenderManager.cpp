#include "RenderManager.h"

RenderManager::RenderManager()
{
	createBuffers();
}

RenderManager::RenderManager(GameScene * otherGameScene, GLFWwindow* otherWindow, ShaderProgramLib* shaderProgram)
{
	gameScene = otherGameScene;
	window = otherWindow;


	this->geometryShaderProgram = shaderProgram->getShader<GeometryShaders>()->geometryShaderProgram;
	this->lightpassShaderProgram = shaderProgram->getShader<LightpassShaders>()->lightpassShaderProgram;
	//this->animationShaderProgram = shaderProgram->getShader<AnimationShaders>()->animationShaderProgram;
	this->shadowMapShaderProgram = shaderProgram->getShader<ShadowMapShader>()->ShadowMapShaderProgram;
	this->UIShaderProgram = shaderProgram->getShader<UIShaders>()->UIShaderProgram;
	this->terrainShaderProgram = shaderProgram->getShader<TerrainShaders>()->TerrainShaderProgram;
	this->mainMenuShaderProgram = shaderProgram->getShader<MainMenuShader>()->MainMenuShaderProgram;
	//createBuffers();
	vao = 0;
	//createBuffers();

	//// CHECK AGAINST GAMESTATE TO NOT LOAD unnecessary DATA
	//createMainMenuBuffer();

}

RenderManager::~RenderManager()
{
}

void RenderManager::FindObjectsToRender() {
	for (unsigned int i = 0; i < gameScene->gameObjects.size(); i++) {
	/*	glm::vec3 vectorToObject = gameScene->gameObjects[0].transform.position - gameScene->gameObjects[i].transform.position;

		float distance = length(vectorToObject);*/

		if (gameScene->gameObjects[i].getIsRenderable() == true) {
			gameObjectsToRender.push_back(&gameScene->gameObjects[i]);
		}

		if (gameScene->gameObjects[i].hasLight == true) {
			lightsToRender.push_back(gameScene->gameObjects[i].lightComponent);
			//rework this
		}
	}
}

void RenderManager::clearObjectsToRender()
{
	while (!gameObjectsToRender.empty())
	{
		gameObjectsToRender.pop_back();
	}
	gameObjectsToRender.clear();
	
	while (!lightsToRender.empty())
	{
		lightsToRender.pop_back();
	}
	lightsToRender.clear();
}

void RenderManager::createBuffers()
{

	//screen size
	glfwGetFramebufferSize(window, &display_w, &display_h);

	//----------========== ShadowMap FBO DIRECTIONAL LIGHTS ==========----------
	glGenFramebuffers(1, &shadowFBO);
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, HIGH_SHADOW, HIGH_SHADOW, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//... Create G-buffers
	//framebufferobject
	glGenFramebuffers(1, &gbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	//g-buffer position
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	//g-buffer albedo
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo, 0);

	//g-buffer normal
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

	glDrawBuffers(3, attachments);

	//... Create and attach depth buffer
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_w, display_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	//... Create Colorbuffer
	glGenFramebuffers(1, &finalFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	glGenTextures(1, &finalColorBuffer);
	glBindTexture(GL_TEXTURE_2D, finalColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;

	//... Create and attach depth and stencil buffer
	glGenRenderbuffers(1, &finalDepthStensil);
	glBindRenderbuffer(GL_RENDERBUFFER, finalDepthStensil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_w, display_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, finalDepthStensil);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, finalDepthStensil);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	//.. Create UI Frame Buffer with UI Texture
	int width, height, nrOfChannels;
	unsigned char * data = stbi_load("uiTextureFlipped.png", &width, &height, &nrOfChannels, 0);

	glGenFramebuffers(1, &UIFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, UIFBO);

	glGenTextures(1, &UITexture);
	glBindTexture(GL_TEXTURE_2D, UITexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load UI Texture from path" << std::endl;
	}

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, UITexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "UI Framebuffer not complete!" << std::endl;
}

void RenderManager::createMainMenuBuffer()
{
	glGenFramebuffers(1, &finalMainMenuFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, finalMainMenuFBO);

	glGenTextures(1, &finalMainMenuFBOTexture);
	glBindTexture(GL_TEXTURE_2D, finalMainMenuFBOTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalMainMenuFBOTexture, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, finalMainMenuFBOTexture);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Main Menu Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] =
	{
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVertexArrayObject);
	glGenBuffers(1, &quadVertexBufferObject);
	glBindVertexArray(quadVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, quadVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void RenderManager::Render() {
	FindObjectsToRender();

	for (int i = 0; i < gameScene->gameObjects.size(); i++)
	{
		/*
		Use gameobject[0]
		use transform->position in gameObject
		*/
		if (gameScene->gameObjects[i].getPlayer() != nullptr)
		{
			glm::vec2 temp = gameScene->gameObjects[i].getPlayer()->setXZ();
			for (int j = 0; j < gameScene->gameObjects.size(); j++)
				if (gameScene->gameObjects[j].getTerrain() != nullptr)
				{
					gameScene->gameObjects[i].getPlayer()->setCurrentHeight(gameScene->gameObjects[j].getTerrain()->calculateY(temp.x, temp.y));
				}
		}
	}

	//... Set view and projection matrix
	view_matrix = gameScene->gameObjects[0].getViewMatrix();
	projection_matrix = glm::perspective(glm::radians(60.0f), float(display_w) / float(display_h), 0.1f, 100.0f);

	glm::mat4 world_matrix = glm::mat4(1);
	//world_matrix = glm::translate(world_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
	//world_matrix = glm::rotate(world_matrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//... Clear Back Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.749, 0.843, 0.823, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//... Clear finalFBO
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//DIRECTIONAL LIGHT SHADOWMAP PASS-----------------------------------------------------------------------------------------------------------------------
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(shadowMapShaderProgram);
	setupMatrices(shadowMapShaderProgram, gameScene->gameObjects[2].transform->position);
	glViewport(0, 0, HIGH_SHADOW, HIGH_SHADOW);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(glGetUniformLocation(shadowMapShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));

	for (unsigned int i = 0; i < gameObjectsToRender.size(); i++)
	{

		gameObjectsToRender[i]->meshFilterComponent->bindVertexArray();
		glDrawArrays(GL_TRIANGLES, 0, gameObjectsToRender[i]->meshFilterComponent->vertexCount);

	}
	for (int i = 0; i < gameScene->gameObjects.size(); i++)
	{
		if (gameScene->gameObjects[i].getTerrain() != nullptr)
		{
			gameScene->gameObjects[i].getTerrain()->bindVertexArray();
			glDrawElements(GL_TRIANGLE_STRIP, gameScene->gameObjects[i].getTerrain()->indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, display_w, display_h);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	
	//... Terrain PASS----------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(geometryShaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
//	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));
	glUseProgram(terrainShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(terrainShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(terrainShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(glGetUniformLocation(terrainShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // enable writing to the stencil buffer
	for (int i = 0; i < gameScene->gameObjects.size(); i++)
	{
		if (gameScene->gameObjects[i].getTerrain() != nullptr)
		{
			gameScene->gameObjects[i].getTerrain()->bindVertexArray();
	
			glDrawElements(GL_TRIANGLE_STRIP, gameScene->gameObjects[i].getTerrain()->indices.size(), GL_UNSIGNED_INT, 0);
			
			
		}
	}
	

	
	
	//... GEOMETRY PASS----------------------------------------------------------------------------------------------------------------------------------------
	//glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(geometryShaderProgram);
	
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));
	
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilMask(0xFF); // enable writing to the stencil buffer
	
	gameObjectsToRender[0]->materialComponent->bindTextures();

	for (unsigned int i = 0; i < gameObjectsToRender.size(); i++)
	{
		if (gameObjectsToRender[i]->meshFilterComponent->meshType == 3)
			glUniform1i(glGetUniformLocation(geometryShaderProgram, "followCamera"), 1);
		else
			glUniform1i(glGetUniformLocation(geometryShaderProgram, "followCamera"), 0);

		gameObjectsToRender[i]->meshFilterComponent->bindVertexArray();

		//...
		world_matrix = glm::translate(world_matrix, gameObjectsToRender[i]->transform->position);
		glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));
		//...

		glDrawArrays(GL_TRIANGLES, 0, gameObjectsToRender[i]->meshFilterComponent->vertexCount);
	}

	//------=====================Animation Pass=======================-------
	//glUseProgram(animationShaderProgram);

	//... Copy Stencil Buffer from gbo to finalFBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFBO);
	glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

	//... LIGHTING PASS----------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	glUseProgram(lightpassShaderProgram);
	setupMatrices(lightpassShaderProgram, gameScene->gameObjects[2].transform->position);

	//CAM pos
	glUniform3fv(glGetUniformLocation(lightpassShaderProgram, "view_position"), 1, glm::value_ptr(gameScene->gameObjects[0].transform->position));

	//Lights
	for (unsigned int i = 0; i < lightsToRender.size(); i++)
	{
		//position
		std::string lightUniform = "lights[" + std::to_string(i) + "].Position";
		glUniform3fv(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), 1, glm::value_ptr(lightsToRender.at(i)->transform.position));

		//Color
		lightUniform = "lights[" + std::to_string(i) + "].Color";
		glUniform3fv(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), 1, glm::value_ptr(lightsToRender.at(i)->color));

		//Attenuation linear
		lightUniform = "lights[" + std::to_string(i) + "].Linear";
		glUniform1f(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), lightsToRender.at(i)->Linear);

		//Attenuation quadratic
		lightUniform = "lights[" + std::to_string(i) + "].Quadratic";
		glUniform1f(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), lightsToRender.at(i)->Quadratic);
	}
	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gPosition"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gAlbedo"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gNormal"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gNormal);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "depthMap"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilMask(0x00); // disable writing to the stencil buffer

	renderQuad();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glDisable(GL_STENCIL_TEST);

	//... UI -----------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(UIShaderProgram);

	glUniform1i(glGetUniformLocation(UIShaderProgram, "theTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, UITexture);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "equipedTexture"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->equipedTexture);

	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture1"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[0]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture2"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[1]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture3"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[2]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture4"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[3]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture5"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[4]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "textTexture"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->textTexture);
	
	glUniform1i(glGetUniformLocation(UIShaderProgram, "SceneTexture"), 8);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, finalColorBuffer);

	glUniform1f(glGetUniformLocation(UIShaderProgram, "hp"), gameScene->gameObjects[0].getPlayer()->hp);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "cold"), gameScene->gameObjects[0].getPlayer()->cold);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "water"), gameScene->gameObjects[0].getPlayer()->water);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "food"), gameScene->gameObjects[0].getPlayer()->food);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "fade"), gameScene->gameObjects[0].getPlayer()->fade);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "textFade"), gameScene->gameObjects[0].getPlayer()->textFade);

	glBindTexture(GL_TEXTURE_2D, finalColorBuffer);

	renderQuad();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	clearObjectsToRender();
	Update();
}

void RenderManager::renderMainMenu()
{
	FindObjectsToRender();

	view_matrix = gameScene->gameObjects[0].getViewMatrix();
	projection_matrix = glm::perspective(glm::radians(60.0f), float(display_w) / float(display_h), 0.1f, 100.0f);



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.749, 0.843, 0.823, 1.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(mainMenuShaderProgram);

	glBindVertexArray(quadVertexArrayObject);

	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "backGroundTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getMenuScene()->backgroundTexture);
	
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "startButtonTexture"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getMenuScene()->startButtonTexture);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "settingsButtonTexture"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getMenuScene()->settingsButtonTexture);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "ExitButtonTexture"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getMenuScene()->exitButtonTexture);


	//gameScene->gameObjects[0].getMenuScene()->renderFrameQuad(mainMenuShaderProgram);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/*glm::vec2 startButtonMinMax[2];
	glm::vec2 settingsButtonMinMax[2];
	glm::vec2 ExitButtonMinMax[2];
	startButtonMinMax[0].x = 430;
	startButtonMinMax[0].y = 290;
	startButtonMinMax[1].x = 855;
	startButtonMinMax[1].y = 330;

	settingsButtonMinMax[0].x = 455;
	settingsButtonMinMax[0].y = 385;
	settingsButtonMinMax[1].x = 830;
	settingsButtonMinMax[1].y = 415;

	ExitButtonMinMax[0].x = 430;
	ExitButtonMinMax[0].y = 475;
	ExitButtonMinMax[1].x = 855;
	ExitButtonMinMax[1].y = 510;

	double xPos;
	double yPos;

	glfwGetCursorPos(window, &xPos, &yPos);

	if (startButtonMinMax[0].x < xPos && xPos < startButtonMinMax[1].x && startButtonMinMax[0].y < yPos && yPos < startButtonMinMax[1].y)
		std::cout << "------------------- CURSOR IS INSIDE STARTBUTTONBOX -------------------" << std::endl;
	else if (settingsButtonMinMax[0].x < xPos && xPos < settingsButtonMinMax[1].x && settingsButtonMinMax[0].y < yPos && yPos < settingsButtonMinMax[1].y)
		std::cout << "------------------- CURSOR IS INSIDE SETTINGSBUTTONBOX -------------------" << std::endl;
	else if (ExitButtonMinMax[0].x < xPos && xPos < ExitButtonMinMax[1].x && ExitButtonMinMax[0].y < yPos && yPos < ExitButtonMinMax[1].y)
		std::cout << "------------------- CURSOR IS INSIDE EXITBUTTONBOX -------------------" << std::endl;
*/

	//gameScene->gameObjects[0].getMenuScene()->deleteObjects();
	clearObjectsToRender(); 
	Update();
}

void RenderManager::renderMenuQuad()
{
}

void RenderManager::renderQuad()
{
	if (vao == 0)
	{
		unsigned int vertexPos;
		unsigned int uvPos;

		vertexPos = glGetAttribLocation(lightpassShaderProgram, "aPos");
		uvPos = glGetAttribLocation(lightpassShaderProgram, "aTexCoords");

		//create vertices
		QuadVertex vertices[] = {
			// pos and normal and uv for each vertex
			{ 1,  1, 1.0f, 1.0f },
			{ 1, -1, 1.0f, 0.0f },
			{ -1, -1, 0.0f, 0.0f },
			{ -1,  1, 0.0f, 1.0f },
		};

		unsigned int indices[] = {
			0,1,3,
			1,2,3,
		};

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);

		if (vertexPos == -1) {
			OutputDebugStringA("Error, can't find aPos attribute in vertex shader\n");
			return;
		}

		glVertexAttribPointer(
			0,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(QuadVertex),
			BUFFER_OFFSET(0)
		);

		glEnableVertexAttribArray(1);

		if (uvPos == -1) {
			OutputDebugStringA("Error, cannt find aTexCoords attribute in vertex shader\n");
			return;
		}

		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(QuadVertex),
			BUFFER_OFFSET(sizeof(float) * 2)
		);

		//ebo
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void RenderManager::setupMatrices(unsigned int shaderToUse, glm::vec3 lightPos)
{
	glUseProgram(shaderToUse);

	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 45.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glUniformMatrix4fv(glGetUniformLocation(shaderToUse, "LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void RenderManager::setupMeshY()
{
	FindObjectsToRender();
	for (int i = 0; i < gameObjectsToRender.size(); i++)
	{
		float yTemp = this->gameScene->gameObjects[1].getTerrain()->calculateY(this->gameObjectsToRender[i]->transform->position.x, this->gameObjectsToRender[i]->transform->position.z);
		this->gameObjectsToRender[i]->transform->position.y = yTemp;
	}
}

void RenderManager::Update()
{
}

void RenderManager::setDeltaTime(float deltaTime)
{
	this->deltaTime = deltaTime;
}

void RenderManager::setSeconds(float seconds)
{
	this->seconds = seconds;
}
