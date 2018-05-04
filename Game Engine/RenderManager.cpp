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
	this->vfxFireShaderProgram = shaderProgram->getShader<VFXFireShaders>()->vfxFireShaderProgram;
	this->vfxSnowShaderProgram = shaderProgram->getShader<VFXSnowShaders>()->vfxSnowShaderProgram;
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
	glm::vec3 vectorToObject = gameScene->gameObjects[0]->transform->position - gameScene->gameObjects[i]->transform->position;

		float distance = length(vectorToObject);

		if (gameScene->gameObjects[i]->getIsRenderable() == true && distance < 83) {
			gameObjectsToRender.push_back(gameScene->gameObjects[i]);
		}

		if (gameScene->gameObjects[i]->hasLight == true) {
			lightsToRender.push_back(gameScene->gameObjects[i]->lightComponent);
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

	//VFX
	fireParticlePositionData = new GLfloat[MAX_PARTICLES * 4];
	snowParticlePositionData = new GLfloat[MAX_PARTICLES * 4];
	fireParticleColorData = new GLubyte[MAX_PARTICLES * 4];
	snowParticleColorData = new GLubyte[MAX_PARTICLES * 4];
	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f
	};

	//Fire Texture
	glGenVertexArrays(1, &fireVAO);
	glBindVertexArray(fireVAO);
	glGenBuffers(1, &fireVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fireVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &fireParticlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, fireParticlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &fireParticleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, fireParticleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	width = 0;
	height = 0;
	nrOfChannels = 0;
	data = stbi_load("ParticleQuad.png", &width, &height, &nrOfChannels, 0);
	glGenTextures(1, &fireTexture);
	glBindTexture(GL_TEXTURE_2D, fireTexture);
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
		std::cout << "Failed to load Billboard Texture from path" << std::endl;
	}
	stbi_image_free(data);
	
	//Snow Texture
	glGenVertexArrays(1, &snowVAO);
	glBindVertexArray(snowVAO);
	glGenBuffers(1, &snowVBO);
	glBindBuffer(GL_ARRAY_BUFFER, snowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &snowParticlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, snowParticlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &snowParticleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, snowParticleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		fireParticleContainer[i].life = -1.0f;
		snowParticleContainer[i].life = -1.0f;
		fireParticleContainer[i].cameraDistance = -1.0f;
		snowParticleContainer[i].cameraDistance = -1.0f;
	}

	width = 0;
	height = 0;
	nrOfChannels = 0;
	data = stbi_load("ParticleSnow.png", &width, &height, &nrOfChannels, 0);
	glGenTextures(1, &snowTexture);
	glBindTexture(GL_TEXTURE_2D, snowTexture);
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
		std::cout << "Failed to load Billboard Texture from path" << std::endl;
	}
	stbi_image_free(data);

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
	width = 0;
	height = 0;
	nrOfChannels = 0;
	data = stbi_load("uiTextureFlipped.png", &width, &height, &nrOfChannels, 0);

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

void RenderManager::deleteData()
{
	if (fireParticlePositionData != nullptr)
	{
		delete fireParticlePositionData;
	}
	if (fireParticleColorData != nullptr)
	{
		delete fireParticleColorData;
	}
	if (snowParticlePositionData != nullptr)
	{
		delete snowParticlePositionData;
	}
	if (snowParticleColorData != nullptr)
	{
		delete snowParticleColorData;
	}
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

void RenderManager::createButtonQuads()
{
	float buttonQuadVertices1[] = 
	{
		-0.3f,  0.1f, 0.0f, 1.0f, //same
		-0.3f, -0.1f, 0.0f, 0.0f,
		0.3f, -0.1f,  1.0f, 0.0f, //same

		-0.3f,  0.1f, 0.0f, 1.0f, //same
		0.3f, -0.1f,  1.0f, 0.0f, //same
		0.3f,  0.1f,  1.0f, 1.0f
	
	};
	float buttonQuadVertices2[] =
	{
		-0.3f,  0.1f,  0.0f, 1.0f,
		-0.3f, -0.1f,  0.0f, 0.0f,
		0.3f, -0.1f,  1.0f, 0.0f,

		-0.3f,  0.1f,  0.0f, 1.0f,
		0.3f, -0.1f,  1.0f, 0.0f,
		0.3f,  0.1f,  1.0f, 1.0f

	};
	float buttonQuadVertices3[] =
	{
		-0.3f,  0.1f, 0.0f, 1.0f,
		-0.3f, -0.1f,  0.0f, 0.0f,
		0.3f, -0.1f,  1.0f, 0.0f,

		-0.3f,  0.1f,  0.0f, 1.0f,
		0.3f, -0.1f,  1.0f, 0.0f,
		0.3f,  0.1f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &buttonVertexArrayObject[0]);
	glGenBuffers(1, &buttonBufferObject[0]);
	glBindVertexArray(buttonVertexArrayObject[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buttonBufferObject[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buttonQuadVertices1), &buttonQuadVertices1, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenVertexArrays(1, &buttonVertexArrayObject[1]);
	glGenBuffers(1, &buttonBufferObject[1]);
	glBindVertexArray(buttonVertexArrayObject[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buttonBufferObject[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buttonQuadVertices2), &buttonQuadVertices2, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenVertexArrays(1, &buttonVertexArrayObject[2]);
	glGenBuffers(1, &buttonBufferObject[2]);
	glBindVertexArray(buttonVertexArrayObject[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buttonBufferObject[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buttonQuadVertices3), &buttonQuadVertices3, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void RenderManager::Render() {
	FindObjectsToRender();

	for (int i = 0; i < gameScene->gameObjects.size(); i++)
	{
		if (gameScene->gameObjects[i]->getPlayer() != nullptr)
		{
			glm::vec2 temp = gameScene->gameObjects[i]->getPlayer()->setXZ();
			for (int j = 0; j < gameScene->gameObjects.size(); j++)
				if (gameScene->gameObjects[j]->getTerrain() != nullptr)
				{
					gameScene->gameObjects[i]->getPlayer()->setCurrentHeight(gameScene->gameObjects[j]->getTerrain()->calculateY(temp.x, temp.y));
				}
		}
	}

	//... Set view and projection matrix
	for (int i = 0; i < gameScene->gameObjects.size(); i++)
	{
		if (gameScene->gameObjects[i]->getPlayer() != nullptr)
		{
			view_matrix = gameScene->gameObjects[i]->getViewMatrix();
			break;
		}
	}
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
	setupMatrices(shadowMapShaderProgram, gameScene->gameObjects[2]->transform->position); //? what is gameObject[2] supposed to be?
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
		if (gameScene->gameObjects[i]->getTerrain() != nullptr)
		{
			gameScene->gameObjects[i]->getTerrain()->bindVertexArray();
			glDrawElements(GL_TRIANGLE_STRIP, gameScene->gameObjects[i]->getTerrain()->indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, display_w, display_h);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	
	//... Terrain PASS-----------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(terrainShaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(terrainShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(glGetUniformLocation(terrainShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(terrainShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // enable writing to the stencil buffer
	for (int i = 0; i < gameScene->gameObjects.size(); i++)
	{
		if (gameScene->gameObjects[i]->getTerrain() != nullptr)
		{
			gameScene->gameObjects[i]->getTerrain()->bindVertexArray();
			
			glDrawElements(GL_TRIANGLE_STRIP, gameScene->gameObjects[i]->getTerrain()->indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	
	//... GEOMETRY PASS----------------------------------------------------------------------------------------------------------------------------------------

	glUseProgram(geometryShaderProgram);
	
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	//glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));

	if (gameObjectsToRender.size() > 0)
		gameObjectsToRender[0]->materialComponent->bindTextures();

	for (unsigned int i = 0; i < gameObjectsToRender.size(); i++)
	{
		//glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "model_matrix"), 1, GL_FALSE, glm::value_ptr(gameObjectsToRender[i]->transform->position));

		if (gameObjectsToRender[i]->meshFilterComponent->meshType == 3)
			glUniform1i(glGetUniformLocation(geometryShaderProgram, "followCamera"), 1);
		else
			glUniform1i(glGetUniformLocation(geometryShaderProgram, "followCamera"), 0);

		
		gameObjectsToRender[i]->meshFilterComponent->bindVertexArray();

		//... Position
		glm::mat4 tempMatrix = glm::mat4(1);
		tempMatrix = glm::translate(glm::mat4(1), gameObjectsToRender[i]->transform->position);
		//... Rotation, not sure if this works (probably not)
		// need to calculate radians from rotation vector from maya
		float oneMinusDot = 1 - glm::dot(gameObjectsToRender[i]->transform->rotation, glm::vec3(0,0,0));
		float F = glm::pow(oneMinusDot, 5.0);
		tempMatrix = glm::rotate(tempMatrix, glm::radians(F), gameObjectsToRender[i]->transform->rotation);
		//...
		glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(tempMatrix));
		glDrawArrays(GL_TRIANGLES, 0, gameObjectsToRender[i]->meshFilterComponent->vertexCount);
	}

	//... VFX--------------------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF); // enable writing to the stencil buffer
	glStencilFunc(GL_ALWAYS, 2, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glm::vec3 cameraPosition(glm::inverse(view_matrix)[3]);

	//... FIRE
	glUseProgram(vfxFireShaderProgram);
	//Particle system location, can be changed dynamically if e.g. a torch is wanted
	defaultX = 536.0f;
	defaultY = -6.5f;
	defaultZ = 601.0f;
	offset = 15.0f;

	//Randomizer for the spawn location
	randomX = defaultX + (rand() % 3000 - 1500.0f) / 1000.0f;
	randomZ = defaultZ + (rand() % 3000 - 1500.0f) / 1000.0f;

	//Create the direction vector from a start and end point
	//and check how far away the particles are.
	targetPoint = glm::vec3(defaultX, defaultY + offset, defaultZ);
	startPoint = glm::vec3(randomX, defaultY, randomZ);
	particlePivot = glm::vec3(defaultX, defaultY, defaultZ);
	directionVec = targetPoint - startPoint;

	//Get a random target point direction
	randomDirectionX = directionVec.x + (rand() % 2000 - 1000) / 3000.0f;
	randomDirectionZ = directionVec.z + (rand() % 2000 - 1000) / 3000.0f;

	directionVec.x = randomDirectionX;
	directionVec.y = directionVec.y / 5.0f;
	directionVec.z = randomDirectionZ;

	//Check if the particles are far away from the player,
	//if too far away --> Don't render
	tempDistance = particlePivot - gameScene->gameObjects[0]->transform->position;
	distanceToParticles = abs((int)tempDistance.x + (int)tempDistance.z);

	if (distanceToParticles <= 50)
	{
		//Create a randomizer so it doesn't spawn all the particles on every frame, if needed
		randomizer = 1;

		if (randomizer == 1)
		{
			if (particleCount <= MAX_PARTICLES)
			{
				for (int i = 0; i < fireParticles; i++)
				{
					lastUsedParticle = FindUnusedParticle(fireParticleContainer, lastUsedParticle);
					int particleIndex = lastUsedParticle;

					fireParticleContainer[particleIndex].life = 1.0f;
					fireParticleContainer[particleIndex].pos = startPoint;

					//Fix the rest constants that's needed for a "living" looking fire.
					//First, create a spread with values from 0.00 -> 1.00
					float spread = (rand() % 100) / 100.0f;
					glm::vec3 mainDir = glm::vec3(0.0f, 0.1f, 0.0f);

					//Complete random
					glm::vec3 randomDir = glm::vec3(
						(sin(rand() % 10 - 10.0f) / 5.0f),
						(sin(rand() % 10 - 10.0f) / 5.0f),
						(sin(rand() % 10 - 10.0f) / 5.0f)
					);

					//Set the new direction for the particle
					fireParticleContainer[particleIndex].speed = mainDir + directionVec / 5.0f;
					//fireParticleContainer[particleIndex].speed = mainDir + randomDir * spread;

					//Set a "fire looking" colour to the particle
					//Test 1
					/*do
					{
						fireParticleContainer[particleIndex].r = rand() % 220;	//256 highest
					} while (fireParticleContainer[particleIndex].r < 140);
					fireParticleContainer[particleIndex].g = rand() % 60;*/

					//Test 2
					fireParticleContainer[particleIndex].r = 255.0f;
					fireParticleContainer[particleIndex].g = 255.0f;

					fireParticleContainer[particleIndex].b = 0;
					fireParticleContainer[particleIndex].a = (rand() % 256) / 3;

					fireParticleContainer[particleIndex].size = ((rand() % 750) / 2000.0f) / 1.5f;
				}
			}
		}

		particleCount = 0;
		//Movement of the new particles
		for (int i = 0; i < MAX_PARTICLES; i++)
		{
			fireParticleContainer[i].life -= deltaTime / 2.0f;
			if (fireParticleContainer[i].life > 0.0f)
			{
				fireParticleContainer[i].speed += glm::vec3(0.0f, -0.1f, 0.0f) * deltaTime * 0.5f;
				fireParticleContainer[i].pos += fireParticleContainer[i].speed / 30.0f;
				fireParticleContainer[i].cameraDistance = glm::length(fireParticleContainer[i].pos - cameraPosition);

				//Set Positions
				fireParticlePositionData[4 * particleCount + 0] = fireParticleContainer[i].pos.x;
				fireParticlePositionData[4 * particleCount + 1] = fireParticleContainer[i].pos.y;
				fireParticlePositionData[4 * particleCount + 2] = fireParticleContainer[i].pos.z;
				fireParticlePositionData[4 * particleCount + 3] = fireParticleContainer[i].size;

				//Set Colors
				fireParticleColorData[4 * particleCount + 0] = fireParticleContainer[i].life * fireParticleContainer[i].r;

				if (fireParticleContainer[i].life > 0.7f)
				{
					fireParticleColorData[4 * particleCount + 1] = (fireParticleContainer[i].life * fireParticleContainer[i].g) / 3.0f;
				}
				else
				{
					fireParticleColorData[4 * particleCount + 1] = (fireParticleContainer[i].life * fireParticleContainer[i].g) / 4.0f;
				}
				
				fireParticleColorData[4 * particleCount + 2] = fireParticleContainer[i].life * fireParticleContainer[i].b;
				fireParticleColorData[4 * particleCount + 3] = (fireParticleContainer[i].a * fireParticleContainer[i].life) * 3.0f;
			}
			else
			{
				fireParticleContainer[i].cameraDistance = -1.0f;
				fireParticlePositionData[4 * particleCount + 3] = 0;	//If dead -> Size = 0
			}
			particleCount++;
		}

		//Update particle information
		glBindBuffer(GL_ARRAY_BUFFER, fireParticlePositionBuffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLfloat), fireParticlePositionData);

		glBindBuffer(GL_ARRAY_BUFFER, fireParticleColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLubyte), fireParticleColorData);

		//Apply Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fireTexture);
		glUniform1i(glGetUniformLocation(vfxFireShaderProgram, "particleTexture"), 0);

		//Get and set matrices
		viewProjectionMatrix = projection_matrix * view_matrix;
		cameraRight_vector = glm::vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
		cameraUp_vector = glm::vec3(view_matrix[0][1], view_matrix[1][2], view_matrix[2][3]);
		glUniform3fv(glGetUniformLocation(vfxFireShaderProgram, "cameraRight_worldspace"), 1, glm::value_ptr(cameraRight_vector));
		glUniform3fv(glGetUniformLocation(vfxFireShaderProgram, "cameraUp_worldspace"), 1, glm::value_ptr(cameraUp_vector));
		glUniformMatrix4fv(glGetUniformLocation(vfxFireShaderProgram, "vp"), 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
		glUniform3fv(glGetUniformLocation(vfxFireShaderProgram, "view_position"), 1, glm::value_ptr(gameScene->gameObjects[0]->transform->position));
		glUniform3fv(glGetUniformLocation(vfxFireShaderProgram, "particlePivot"), 1, glm::value_ptr(startPoint));

		//Draw Particles
		renderFireParticles();
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);
	}

	//... SNOW
	glUseProgram(vfxSnowShaderProgram);
	//Particle system location, can be changed dynamically if e.g. a torch is wanted
	defaultX = gameScene->gameObjects[0]->transform->position.x;
	defaultY = gameScene->gameObjects[0]->transform->position.y;
	defaultZ = gameScene->gameObjects[0]->transform->position.z;
	offset = 15.0f;

	//Create the direction vector from a start and end point
	//and check how far away the particles are.
	targetPoint = glm::vec3(defaultX, defaultY + offset, defaultZ);
	startPoint = glm::vec3(defaultX, defaultY, defaultZ);
	particlePivot = glm::vec3(defaultX, defaultY, defaultZ);
	directionVec = targetPoint - startPoint;

	//Get a random target point direction
	randomDirectionX = directionVec.x + (rand() % 2000 - 1000) / 3000.0f;
	randomDirectionZ = directionVec.z + (rand() % 2000 - 1000) / 3000.0f;

	directionVec.x = randomDirectionX;
	directionVec.y = directionVec.y / 5.0f;
	directionVec.z = randomDirectionZ;

	//Check if the particles are far away from the player,
	//if too far away --> Don't render
	tempDistance = particlePivot - gameScene->gameObjects[0]->transform->position;
	distanceToParticles = abs((int)tempDistance.x + (int)tempDistance.z);

	//Create a randomizer so it doesn't spawn all the particles on every frame
	randomizer = 1;

	if (randomizer == 1)
	{
		if (particleCount <= MAX_PARTICLES)
		{
			for (int i = 0; i < snowParticles; i++)
			{
				//Randomizer for the spawn location
				randomX = defaultX + (rand() % 6000 - 3000.0f) / 100.0f;
				randomY = defaultY + (rand() % 6000 - 3000.0f) / 100.0f;
				randomZ = defaultZ + (rand() % 6000 - 3000.0f) / 100.0f;

				lastUsedParticle = FindUnusedParticle(snowParticleContainer, lastUsedParticle);
				int particleIndex = lastUsedParticle;

				snowParticleContainer[particleIndex].life = rand() % 3 + 1;
				snowParticleContainer[particleIndex].pos = glm::vec3(randomX, randomY, randomZ);

				//Fix the rest constants that's needed for a "living" looking fire.
				//First, create a spread with values from 0.00 -> 1.00
				float spread = (rand() % 100) / 100.0f;
				glm::vec3 mainDir = glm::vec3(0.0f, -0.1f, 0.0f);

				//Complete random
				glm::vec3 randomDir = glm::vec3(
					(sin(rand() % 10 - 10.0f) / 5.0f),
					0,
					(sin(rand() % 10 - 10.0f) / 5.0f)
				);

				//Set the new direction for the particle
				//fireParticleContainer[particleIndex].speed = mainDir + directionVec / 5.0f;
				snowParticleContainer[particleIndex].speed = mainDir + randomDir * spread;

				//Set a "fire looking" colour to the particle
				//Test 1
				/*do
				{
				fireParticleContainer[particleIndex].r = rand() % 220;	//256 highest
				} while (fireParticleContainer[particleIndex].r < 140);
				fireParticleContainer[particleIndex].g = rand() % 60;*/

				//Test 2
				/*fireParticleContainer[particleIndex].r = 150.0f;
				fireParticleContainer[particleIndex].g = 150.0f;
				fireParticleContainer[particleIndex].b = 150.0f;*/
				snowParticleContainer[particleIndex].a = (rand() % 256) / 3;

				snowParticleContainer[particleIndex].size = ((rand() % 750) / 2000.0f) / 1.5f;
			}
		}
	}

	particleCount = 0;
	//Movement of the new particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		snowParticleContainer[i].life -= deltaTime / 2.0f;
		if (snowParticleContainer[i].life > 0.0f)
		{
			snowParticleContainer[i].speed += glm::vec3(0.0f, -0.3f, 0.0f) * deltaTime * 0.5f;
			snowParticleContainer[i].pos += snowParticleContainer[i].speed / 30.0f;
			snowParticleContainer[i].cameraDistance = glm::length(snowParticleContainer[i].pos - cameraPosition);

			//Set Positions
			snowParticlePositionData[4 * particleCount + 0] = snowParticleContainer[i].pos.x;
			snowParticlePositionData[4 * particleCount + 1] = snowParticleContainer[i].pos.y;
			snowParticlePositionData[4 * particleCount + 2] = snowParticleContainer[i].pos.z;

			if (snowParticleContainer[i].life > 1.0f)
			{
				snowParticlePositionData[4 * particleCount + 3] = snowParticleContainer[i].size;
			}
			else
			{
				snowParticlePositionData[4 * particleCount + 3] = snowParticleContainer[i].size * snowParticleContainer[i].life;
			}

			//Set Colors
			snowParticleColorData[4 * particleCount + 0] = snowParticleContainer[i].r;
			snowParticleColorData[4 * particleCount + 1] = snowParticleContainer[i].g;
			snowParticleColorData[4 * particleCount + 2] = snowParticleContainer[i].b;
			snowParticleColorData[4 * particleCount + 3] = snowParticleContainer[i].a;
		}
		else
		{
			snowParticleContainer[i].cameraDistance = -1.0f;
			snowParticlePositionData[4 * particleCount + 3] = 0;	//If dead -> Size = 0
		}
		particleCount++;
	}

	//Update particle information
	glBindBuffer(GL_ARRAY_BUFFER, snowParticlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLfloat), snowParticlePositionData);

	glBindBuffer(GL_ARRAY_BUFFER, snowParticleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLubyte), snowParticleColorData);

	//Apply Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snowTexture);
	glUniform1i(glGetUniformLocation(vfxSnowShaderProgram, "particleTexture"), 0);

	//Get and set matrices
	viewProjectionMatrix = projection_matrix * view_matrix;
	cameraRight_vector = glm::vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
	cameraUp_vector = glm::vec3(view_matrix[0][1], view_matrix[1][2], view_matrix[2][3]);
	glUniform3fv(glGetUniformLocation(vfxSnowShaderProgram, "cameraRight_worldspace"), 1, glm::value_ptr(cameraRight_vector));
	glUniform3fv(glGetUniformLocation(vfxSnowShaderProgram, "cameraUp_worldspace"), 1, glm::value_ptr(cameraUp_vector));
	glUniformMatrix4fv(glGetUniformLocation(vfxSnowShaderProgram, "vp"), 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
	glUniform3fv(glGetUniformLocation(vfxSnowShaderProgram, "view_position"), 1, glm::value_ptr(gameScene->gameObjects[0]->transform->position));
	glUniform3fv(glGetUniformLocation(vfxSnowShaderProgram, "particlePivot"), 1, glm::value_ptr(startPoint));

	//Draw Particles
	renderSnowParticles();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);
	

	//... Copy Stencil Buffer from gbo to finalFBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFBO);
	glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//... LIGHTING PASS----------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	glUseProgram(lightpassShaderProgram);
	setupMatrices(lightpassShaderProgram, gameScene->gameObjects[2]->transform->position);

	//CAM pos
	glUniform3fv(glGetUniformLocation(lightpassShaderProgram, "view_position"), 1, glm::value_ptr(gameScene->gameObjects[0]->transform->position));

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
	for (int i = 0; i < gameScene->gameObjects.size(); i++)
	{
		if (gameScene->gameObjects[i]->getPlayer() != nullptr)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glUseProgram(UIShaderProgram);

			glUniform1i(glGetUniformLocation(UIShaderProgram, "theTexture"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, UITexture);
			glUniform1i(glGetUniformLocation(UIShaderProgram, "equipedTexture"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getPlayer()->equipedTexture);

			glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture1"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getPlayer()->inventoryTexture[0]);
			glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture2"), 3);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getPlayer()->inventoryTexture[1]);
			glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture3"), 4);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getPlayer()->inventoryTexture[2]);
			glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture4"), 5);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getPlayer()->inventoryTexture[3]);
			glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture5"), 6);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getPlayer()->inventoryTexture[4]);
			glUniform1i(glGetUniformLocation(UIShaderProgram, "textTexture"), 7);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getPlayer()->textTexture);

			glUniform1i(glGetUniformLocation(UIShaderProgram, "SceneTexture"), 8);
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, finalColorBuffer);

			glUniform1f(glGetUniformLocation(UIShaderProgram, "hp"), gameScene->gameObjects[0]->getPlayer()->hp);
			glUniform1f(glGetUniformLocation(UIShaderProgram, "cold"), gameScene->gameObjects[0]->getPlayer()->cold);
			glUniform1f(glGetUniformLocation(UIShaderProgram, "water"), gameScene->gameObjects[0]->getPlayer()->water);
			glUniform1f(glGetUniformLocation(UIShaderProgram, "food"), gameScene->gameObjects[0]->getPlayer()->food);
			glUniform1f(glGetUniformLocation(UIShaderProgram, "fade"), gameScene->gameObjects[0]->getPlayer()->fade);
			glUniform1f(glGetUniformLocation(UIShaderProgram, "textFade"), gameScene->gameObjects[0]->getPlayer()->textFade);

			glBindTexture(GL_TEXTURE_2D, finalColorBuffer);
			renderQuad();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			break;
		}
	}
	clearObjectsToRender();
	Update();
}

void RenderManager::renderMainMenu()
{
	FindObjectsToRender();

	view_matrix = gameScene->gameObjects[0]->getViewMatrix();
	projection_matrix = glm::perspective(glm::radians(60.0f), float(display_w) / float(display_h), 0.1f, 100.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.749, 0.843, 0.823, 1.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(mainMenuShaderProgram);

	gameScene->gameObjects[0]->getMenuScene()->buttonTransformations = 1;
	glBindVertexArray(buttonVertexArrayObject[0]);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "textureToUse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getMenuScene()->startButtonTexture);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "buttonTransformation"), gameScene->gameObjects[0]->getMenuScene()->buttonTransformations);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling1"), gameScene->gameObjects[0]->getMenuScene()->scaling1);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling2"), gameScene->gameObjects[0]->getMenuScene()->scaling2);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling3"), gameScene->gameObjects[0]->getMenuScene()->scaling3);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	gameScene->gameObjects[0]->getMenuScene()->buttonTransformations = 2;
	glBindVertexArray(buttonVertexArrayObject[1]);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "textureToUse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getMenuScene()->settingsButtonTexture);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "buttonTransformation"), gameScene->gameObjects[0]->getMenuScene()->buttonTransformations);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling1"), gameScene->gameObjects[0]->getMenuScene()->scaling1);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling2"), gameScene->gameObjects[0]->getMenuScene()->scaling2);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling3"), gameScene->gameObjects[0]->getMenuScene()->scaling3);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	gameScene->gameObjects[0]->getMenuScene()->buttonTransformations = 3;
	glBindVertexArray(buttonVertexArrayObject[2]);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "textureToUse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getMenuScene()->exitButtonTexture);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "buttonTransformation"), gameScene->gameObjects[0]->getMenuScene()->buttonTransformations);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling1"), gameScene->gameObjects[0]->getMenuScene()->scaling1);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling2"), gameScene->gameObjects[0]->getMenuScene()->scaling2);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling3"), gameScene->gameObjects[0]->getMenuScene()->scaling3);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	gameScene->gameObjects[0]->getMenuScene()->buttonTransformations = 0;
	glBindVertexArray(quadVertexArrayObject);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "textureToUse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0]->getMenuScene()->backgroundTexture);
	glUniform1i(glGetUniformLocation(mainMenuShaderProgram, "buttonTransformation"), gameScene->gameObjects[0]->getMenuScene()->buttonTransformations);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling1"), gameScene->gameObjects[0]->getMenuScene()->scaling1);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling2"), gameScene->gameObjects[0]->getMenuScene()->scaling2);
	glUniform1f(glGetUniformLocation(mainMenuShaderProgram, "scaling3"), gameScene->gameObjects[0]->getMenuScene()->scaling3);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//gameScene->gameObjects[0].getMenuScene()->deleteObjects();
	clearObjectsToRender(); 
	Update();
	glDisable(GL_BLEND);
}

void RenderManager::renderQuad()
{
	if (vao == 0)
	{
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

void RenderManager::renderFireParticles()
{
	glBindVertexArray(fireVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, fireVBO);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	//Positions : center
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, fireParticlePositionBuffer);
	glVertexAttribPointer(
		1,
		4,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//Colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, fireParticleColorBuffer);
	glVertexAttribPointer(
		2,
		4,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		0,
		(void*)0
	);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
}

void RenderManager::renderSnowParticles()
{
	glBindVertexArray(snowVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, snowVBO);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	//Positions : center
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, snowParticlePositionBuffer);
	glVertexAttribPointer(
		1,
		4,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//Colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, snowParticleColorBuffer);
	glVertexAttribPointer(
		2,
		4,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		0,
		(void*)0
	);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
}

void RenderManager::ParticleLinearSort(Particle* arr, int size)
{
	int a, b, key;
	for (a = 0; a < size; a++)
	{
		key = arr[a].life;
		b = a - 1;

		while (b >= 0 && arr[b].life > key)
		{
			arr[b + 1] = arr[b];
			b = b - 1;
		}
		arr[b + 1].life = key;
	}
}

int RenderManager::FindUnusedParticle(Particle* container, int lastUsedParticle)
{
	for (int i = lastUsedParticle; i < MAX_PARTICLES; i++)
	{
		if (container[i].life < 0)
		{
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (container[i].life < 0)
		{
			return i;
		}
	}
	return 0;
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