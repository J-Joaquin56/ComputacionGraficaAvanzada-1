#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;

std::shared_ptr<ThirdPersonCamera> camera(new ThirdPersonCamera());
float distanceFromPlayer = 6.5f;
float angleTarget = 0.0f;
glm::vec3 positionTarget;

std::shared_ptr<FirstPersonCamera> firstPersonCamera(new FirstPersonCamera()); 	// Cámara en primera persona
bool isFirstPersonActive = false;  												// Para controlar qué cámara está activa

Sphere skyboxSphere(20, 20);
Box boxCesped;
Box boxWalls;
Box boxHighway;
Box boxLandingPad;
Sphere esfera1(10, 10);

// Lamps
Model modelLamp1;
Model modelLamp2;
Model modelLampPost2;

// Modelos animados

// Kakashi
Model modelKakashiReversa;
Model modelKakashiCaminando;
Model modelKakashiCorriendo;
Model modelKakashiQuieto;

// Kratos
Model modelKratosReversa;
Model modelKratosCaminando;
Model modelKratosCorriendo;
Model modelKratosQuieto;

// Naruto
Model modelNarutoReversa;
Model modelNarutoCaminando;
Model modelNarutoCorriendo;
Model modelNarutoQuieto;

// Teletransporte
Model modelCirculoMagico;

// Terrain model instance
Terrain island1(-1, -1, 200, 8, "../proyecto/textures/heightmap_practica05.png");
Terrain island2(-1, -1, 200, 8, "../proyecto/textures/heightmap_exercise2.png");
Terrain island3(-1, -1, 200, 8, "../proyecto/textures/heightmap.png");


GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureIsland1RID, textureIsland1GID, textureIsland1BID, textureIsland1BlendMapID;
GLuint textureIsland2RID, textureIsland2GID, textureIsland2BID, textureIsland2BlendMapID;
GLuint textureIsland3RID, textureIsland3GID, textureIsland3BID, textureIsland3BlendMapID;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { 
		"../proyecto/skybox_proyecto/front.png",
		"../proyecto/skybox_proyecto/back.png",
		"../proyecto/skybox_proyecto/top.png",
		"../proyecto/skybox_proyecto/bottom.png",
		"../proyecto/skybox_proyecto/right.png",
		"../proyecto/skybox_proyecto/left.png"};

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 matrixModelCirculo = glm::mat4(1.0);
glm::mat4 modelMatrixKakashi = glm::mat4(1.0f);
glm::mat4 modelMatrixKratos = glm::mat4(1.0f);
glm::mat4 modelMatrixNaruto = glm::mat4(1.0f);

uint8_t modelSelected = 0;
bool enableCountSelected = true;
enum Personaje {
	KAKASHI = 0,
	KRATOS = 1,
	NARUTO = 2
};
uint8_t numeroPersonajesIntercambiar = 3;
uint8_t kakashiState = 0;
uint8_t kratosState = 0;
uint8_t narutoState = 0;

// Lamps position
std::vector<glm::vec3> lamp1Position = {
	glm::vec3(-7.03, 0, -19.14),
	glm::vec3(24.41, 0, -34.57),
	glm::vec3(-10.15, 0, -54.1)
};
std::vector<float> lamp1Orientation = {
	-17.0, -82.67, 23.70
};
std::vector<glm::vec3> lamp2Position = {
	glm::vec3(-36.52, 0, -23.24),
	glm::vec3(-52.73, 0, -3.90)
};
std::vector<float> lamp2Orientation = {
	21.37 + 90, -65.0 + 90
};

double deltaTime;
double currTime, lastTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);

// Firma del metodo para usar el scroll
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void GenerarTextura(Texture texture, GLuint &textureID);
void RenderTextura(GLuint Cesped, GLuint R, GLuint G, GLuint B, GLuint BlendMap);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scrollCallback);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation.vs", "../Shaders/multipleLights.fs");
	shaderTerrain.initialize("../Shaders/terrain.vs", "../Shaders/terrain.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	boxCesped.init();
	boxCesped.setShader(&shaderMulLighting);

	boxWalls.init();
	boxWalls.setShader(&shaderMulLighting);

	boxHighway.init();
	boxHighway.setShader(&shaderMulLighting);

	boxLandingPad.init();
	boxLandingPad.setShader(&shaderMulLighting);

	esfera1.init();
	esfera1.setShader(&shaderMulLighting);

	//Lamps models
	modelLamp1.loadModel("../models/Street-Lamp-Black/objLamp.obj");
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.loadModel("../models/Street_Light/Lamp.obj");
	modelLamp2.setShader(&shaderMulLighting);
	modelLampPost2.loadModel("../models/Street_Light/LampPost.obj");
	modelLampPost2.setShader(&shaderMulLighting);

	// Kakashi
	modelKakashiReversa.loadModel("../proyecto/models/kakashi/KakashiReversa.fbx");
	modelKakashiReversa.setShader(&shaderMulLighting);
	modelKakashiCaminando.loadModel("../proyecto/models/kakashi/KakashiCaminando.fbx");
	modelKakashiCaminando.setShader(&shaderMulLighting);
	modelKakashiCorriendo.loadModel("../proyecto/models/kakashi/KakashiRun.fbx");
	modelKakashiCorriendo.setShader(&shaderMulLighting);
	modelKakashiQuieto.loadModel("../proyecto/models/kakashi/KakashiQuieto.fbx");
	modelKakashiQuieto.setShader(&shaderMulLighting);
	
	// Kratos
	modelKratosReversa.loadModel("../proyecto/models/kratos/KratosReversa.fbx");
	modelKratosReversa.setShader(&shaderMulLighting);
	modelKratosCaminando.loadModel("../proyecto/models/kratos/KratosCaminando.fbx");
	modelKratosCaminando.setShader(&shaderMulLighting);
	modelKratosCorriendo.loadModel("../proyecto/models/kratos/KratosRun.fbx");
	modelKratosCorriendo.setShader(&shaderMulLighting);
	modelKratosQuieto.loadModel("../proyecto/models/kratos/KratosQuieto.fbx");
	modelKratosQuieto.setShader(&shaderMulLighting);

	// Naruto
	modelNarutoReversa.loadModel("../proyecto/models/naruto/NarutoReversa.fbx");
	modelNarutoReversa.setShader(&shaderMulLighting);
	modelNarutoCaminando.loadModel("../proyecto/models/naruto/NarutoCaminando.fbx");
	modelNarutoCaminando.setShader(&shaderMulLighting);
	modelNarutoCorriendo.loadModel("../proyecto/models/naruto/NarutoRun.fbx");
	modelNarutoCorriendo.setShader(&shaderMulLighting);
	modelNarutoQuieto.loadModel("../proyecto/models/naruto/NarutoQuieto.fbx");
	modelNarutoQuieto.setShader(&shaderMulLighting);

	// Circulo de transporte
	modelCirculoMagico.loadModel("../models/Proyecto/teletransportador/circulo_magico.obj");
	modelCirculoMagico.setShader(&shaderMulLighting);

	// Terreno
	island1.init();
	island1.setShader(&shaderTerrain);
	island2.init();
	island2.setShader(&shaderTerrain);
	island3.init();
	island3.setShader(&shaderTerrain);

	camera->setPosition(glm::vec3(0.0, 3.0, 4.0));
	firstPersonCamera->setPosition(glm::vec3(0.0f, 6.0f, 8.0f));
	
	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		skyboxTexture.loadImage(true);
		if (skyboxTexture.getData()) {
			glTexImage2D(types[i], 0, skyboxTexture.getChannels() == 3 ? GL_RGB : GL_RGBA, skyboxTexture.getWidth(), skyboxTexture.getHeight(), 0,
			skyboxTexture.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, skyboxTexture.getData());
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage();
	}

	// grassy2
	Texture textureCesped("../proyecto/textures/grassy2.png");
	GenerarTextura(textureCesped, textureCespedID);

	// Island1
	Texture texture1R("../proyecto/textures/tierra.png");
	GenerarTextura(texture1R, textureIsland1RID);
	Texture texture1G("../proyecto/textures/flores.png");
	GenerarTextura(texture1G, textureIsland1GID);
	Texture texture1B("../proyecto/textures/path.png");
	GenerarTextura(texture1B, textureIsland1BID);
	Texture texture1BlendMap("../proyecto/textures/blendMap_Practica04.png");
	GenerarTextura(texture1BlendMap, textureIsland1BlendMapID);

	// Island2
	Texture texture2R("../proyecto/textures/tierra.png");
	GenerarTextura(texture2R, textureIsland2RID);
	Texture texture2G("../proyecto/textures/flores.png");
	GenerarTextura(texture2G, textureIsland2GID);
	Texture texture2B("../proyecto/textures/path.png");
	GenerarTextura(texture2B, textureIsland2BID);
	Texture texture2BlendMap("../proyecto/textures/blendMap.png");
	GenerarTextura(texture2BlendMap, textureIsland2BlendMapID);

	// Island3
	Texture texture3R("../proyecto/textures/tierra.png");
	GenerarTextura(texture3R, textureIsland3RID);
	Texture texture3G("../proyecto/textures/flores.png");
	GenerarTextura(texture3G, textureIsland3GID);
	Texture texture3B("../proyecto/textures/path.png");
	GenerarTextura(texture3B, textureIsland3BID);
	Texture texture3BlendMap("../proyecto/textures/blendMap_Practica05.png");
	GenerarTextura(texture3BlendMap, textureIsland3BlendMapID);
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCesped.destroy();
	boxWalls.destroy();
	boxHighway.destroy();
	boxLandingPad.destroy();
	esfera1.destroy();

	// Custom objects Delete
	modelLamp1.destroy();
	modelLamp2.destroy();
	modelLampPost2.destroy();
	modelKakashiReversa.destroy();
	modelKakashiCaminando.destroy();
	modelKakashiCorriendo.destroy();
	modelKakashiQuieto.destroy();
	modelKratosReversa.destroy();
	modelKratosCaminando.destroy();
	modelKratosCorriendo.destroy();
	modelKratosQuieto.destroy();
	modelNarutoReversa.destroy();
	modelNarutoCaminando.destroy();
	modelNarutoCorriendo.destroy();
	modelNarutoQuieto.destroy();
	modelCirculoMagico.destroy();

	// Terrains objects Delete
	island1.destroy();
	island2.destroy();
	island3.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureIsland1BID);
	glDeleteTextures(1, &textureIsland1GID);
	glDeleteTextures(1, &textureIsland1RID);
	glDeleteTextures(1, &textureIsland1BlendMapID);
	glDeleteTextures(1, &textureIsland2BID);
	glDeleteTextures(1, &textureIsland2GID);
	glDeleteTextures(1, &textureIsland2RID);
	glDeleteTextures(1, &textureIsland2BlendMapID);
	glDeleteTextures(1, &textureIsland3BID);
	glDeleteTextures(1, &textureIsland3GID);
	glDeleteTextures(1, &textureIsland3RID);
	glDeleteTextures(1, &textureIsland3BlendMapID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                exitApp = true;
                break;
            // Alternar entre la cámara en tercera persona y la cámara en primera persona
            case GLFW_KEY_K:
                if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
                    isFirstPersonActive = !isFirstPersonActive;  // Apagar, encender la cámara
                }
                break;
        }
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	static bool firstMouse = true;
    if (firstMouse) {
        lastMousePosX = xpos;
        lastMousePosY = ypos;
        firstMouse = false;
    }

    offsetX = xpos - lastMousePosX;
    offsetY = ypos - lastMousePosY;
    lastMousePosX = xpos;
    lastMousePosY = ypos;

    camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
	firstPersonCamera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	distanceFromPlayer -= yoffset;
	camera->setDistanceFromTarget(distanceFromPlayer);
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	// Si la cámara en primera persona está activa
    if (isFirstPersonActive) {
        // Factor de velocidad para acelerar el movimiento de la cámara
		float speedMultiplier = 2.0f; // Para aumentar la velocidad
		float superSpeedMultiplier = 5.0f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			firstPersonCamera->moveFrontCamera(true, deltaTime * speedMultiplier);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			firstPersonCamera->moveFrontCamera(false, deltaTime * speedMultiplier);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			firstPersonCamera->moveRightCamera(false, deltaTime * speedMultiplier);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			firstPersonCamera->moveRightCamera(true, deltaTime * speedMultiplier);
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				firstPersonCamera->moveFrontCamera(true, deltaTime * superSpeedMultiplier);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				firstPersonCamera->moveFrontCamera(false, deltaTime * superSpeedMultiplier);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				firstPersonCamera->moveRightCamera(false, deltaTime * superSpeedMultiplier);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				firstPersonCamera->moveRightCamera(true, deltaTime * superSpeedMultiplier);
		}
    }

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0f, deltaTime);

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) 
		camera->mouseMoveCamera(0.0f, offsetY, deltaTime);

	offsetX = 0;
	offsetY = 0;
	kakashiState = 0;
	kratosState = 0;
	narutoState = 0;

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS){
		enableCountSelected = false;
		modelSelected = ++modelSelected % numeroPersonajesIntercambiar; 
	} else if(glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
		enableCountSelected = true;
	}

	// Controles para mover izquierda y a la derecha
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		angleTarget += 0.02f;
		if(modelSelected == Personaje::KAKASHI) {
			modelMatrixKakashi = glm::rotate(modelMatrixKakashi, 0.02f, glm::vec3(0, 1, 0));
		} 
		else if(modelSelected == Personaje::KRATOS) {
			modelMatrixKratos = glm::rotate(modelMatrixKratos, 0.02f, glm::vec3(0, 1, 0));
		} 
		else if(modelSelected == Personaje::NARUTO) {
			modelMatrixNaruto = glm::rotate(modelMatrixNaruto, 0.02f, glm::vec3(0, 1, 0));
		}
	} 
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		angleTarget -= 0.02f;
		
		if(modelSelected == Personaje::KAKASHI) {
			modelMatrixKakashi = glm::rotate(modelMatrixKakashi, -0.02f, glm::vec3(0, 1, 0));
		}
		else if(modelSelected == Personaje::KRATOS) {
			modelMatrixKratos = glm::rotate(modelMatrixKratos, -0.02f, glm::vec3(0, 1, 0));
		} 
		else if(modelSelected == Personaje::NARUTO) {
			modelMatrixNaruto = glm::rotate(modelMatrixNaruto, -0.02f, glm::vec3(0, 1, 0));
		}
	}

	// Controles para ir adelante y atras
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (modelSelected == Personaje::KAKASHI) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				kakashiState = 2; // Estado de correr
				modelMatrixKakashi = glm::translate(modelMatrixKakashi, glm::vec3(0.0, 0.0, 20.0));
			} else {
				kakashiState = 1; // Estado de caminar
				modelMatrixKakashi = glm::translate(modelMatrixKakashi, glm::vec3(0.0, 0.0, 5.0));
			}
		}
		else if (modelSelected == Personaje::KRATOS) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				kratosState = 2; // Estado de correr
				modelMatrixKratos = glm::translate(modelMatrixKratos, glm::vec3(0.0, 0.0, 20.0));
			} else {
				kratosState = 1; // Estado de caminar
				modelMatrixKratos = glm::translate(modelMatrixKratos, glm::vec3(0.0, 0.0, 5.0));
			}
		}
		else if (modelSelected == Personaje::NARUTO) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				narutoState = 2; // Estado de correr
				modelMatrixNaruto = glm::translate(modelMatrixNaruto, glm::vec3(0.0, 0.0, 20.0));
			} else {
				narutoState = 1; // Estado de caminar
				modelMatrixNaruto = glm::translate(modelMatrixNaruto, glm::vec3(0.0, 0.0, 5.0));
			}
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if(modelSelected == Personaje::KAKASHI) {
			kakashiState = 3;
			modelMatrixKakashi = glm::translate(modelMatrixKakashi, glm::vec3(0.0, 0.0, -5.0));
		}
		else if(modelSelected == Personaje::KRATOS) {
			kratosState = 3;
			modelMatrixKratos = glm::translate(modelMatrixKratos, glm::vec3(0.0, 0.0, -5.0));
		}
		else if(modelSelected == Personaje::NARUTO) {
			narutoState = 3;
			modelMatrixNaruto = glm::translate(modelMatrixNaruto, glm::vec3(0.0, 0.0, -5.0));
		}
	}
	/* if(modelSelected == Personaje::KAKASHI && glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		kakashiState = 2; } */

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	int state = 0;
	float advanceCount = 0.0;
	float rotCount = 0.0;
	int numberAdvance = 0;
	int maxAdvance = 0.0;

	modelMatrixKakashi = glm::translate(modelMatrixKakashi, glm::vec3(10.0f, 0.05f, 0.0f));
	modelMatrixKakashi = glm::scale(modelMatrixKakashi, glm::vec3(0.01f));
	modelMatrixKakashi = glm::rotate(modelMatrixKakashi, glm::radians(0.0f), glm::vec3(0, 1, 0));

	modelMatrixKratos = glm::translate(modelMatrixKratos, glm::vec3(20.0f, 0.05f, 0.0f));
	modelMatrixKratos = glm::scale(modelMatrixKratos, glm::vec3(0.01f));
	modelMatrixKratos = glm::rotate(modelMatrixKratos, glm::radians(0.0f), glm::vec3(0, 1, 0));

	modelMatrixNaruto = glm::translate(modelMatrixNaruto, glm::vec3(30.0f, 0.05f, 0.0f));
	modelMatrixNaruto = glm::scale(modelMatrixNaruto, glm::vec3(0.01f));
	modelMatrixNaruto = glm::rotate(modelMatrixNaruto, glm::radians(0.0f), glm::vec3(0, 1, 0));

	lastTime = TimeManager::Instance().GetTime();

	// Inicializacoin de valores de la camara
	camera->setSensitivity(1.2f);
	camera->setDistanceFromTarget(distanceFromPlayer);

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 1000.0f);

		if(modelSelected == 0){
			positionTarget = modelMatrixKakashi[3];
			positionTarget.y += 3.0f;
		}
		else if(modelSelected == 1){
			positionTarget = modelMatrixKratos[3];
			positionTarget.y += 3.0f;
			positionTarget.x += 1.0f;
		}

		else if(modelSelected == 2){
			positionTarget = modelMatrixNaruto[3];
			positionTarget.y += 2.0f;
		}

		glm::mat4 view;
		if (isFirstPersonActive) {
            view = firstPersonCamera->getViewMatrix();  // Usar cámara en primera persona
        } else {
            camera->setCameraTarget(positionTarget);
			camera->setAngleTarget(angleTarget);
			camera->updateCamera();
			view = camera->getViewMatrix();
        }

		// Después de actualizar la cámara
        float angleAroundTarget = camera->getAngleAroundTarget();
        float maxAngleDifference = glm::radians(60.0f); // Ángulo máximo antes de rotar el personaje
        float characterRotation = 0.0f;

        if (angleAroundTarget > maxAngleDifference) {
            characterRotation = angleAroundTarget - maxAngleDifference;
        } else if (angleAroundTarget < -maxAngleDifference) {
            characterRotation = angleAroundTarget + maxAngleDifference;
        }

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
				glm::value_ptr(view));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderTerrain.setInt("spotLightCount", 1);
		glm::vec3 spotPosition = glm::vec3(modelMatrixKakashi * glm::vec4(0.0, 0.2, 1.75, 1.0));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.07);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		shaderTerrain.setFloat("spotLights[0].linear", 0.07);
		shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", lamp1Position.size() + lamp2Position.size());
		shaderTerrain.setInt("pointLightCount", lamp1Position.size() + lamp2Position.size());
		for(int i = 0; i < lamp1Position.size(); i++){
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp1Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(0.5));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0.0, 10.35, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
		}
		for(int i = 0; i < lamp2Position.size(); i++){
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp2Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(1.0));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0.75, 5.0, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].quadratic", 0.02);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].quadratic", 0.02);
		}

		/*******************************************
		 * Terrain Cesped
		 *******************************************/
		RenderTextura(textureCespedID, textureIsland1RID, textureIsland1GID, textureIsland1BID, textureIsland1BlendMapID);
		//glDisable(GL_CULL_FACE);
		island1.setPosition(glm::vec3(100, 0, 100));
		island1.render();
		//glEnable(GL_CULL_FACE);

		RenderTextura(textureCespedID, textureIsland2RID, textureIsland2GID, textureIsland2BID, textureIsland2BlendMapID);
		//glDisable(GL_CULL_FACE);
		island2.setPosition(glm::vec3(100, 150, 100));
		island2.setOrientation(glm::vec3(0, 0, 45));
		island2.render();
		//glEnable(GL_CULL_FACE);

		RenderTextura(textureCespedID, textureIsland3RID, textureIsland3GID, textureIsland3BID, textureIsland3BlendMapID);
		//glDisable(GL_CULL_FACE);
		island3.setPosition(glm::vec3(100, 150, 100));
		island3.setOrientation(glm::vec3(0, 0, 135));
		island3.render();
		//glEnable(GL_CULL_FACE);
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		/*******************************************
		 * Custom objects obj
		 *******************************************/
		//Magic Circle Rende
		matrixModelCirculo[3][1] = island1.getHeightTerrain(matrixModelCirculo[3][0], matrixModelCirculo[3][2]);
		matrixModelCirculo = glm::translate(matrixModelCirculo, glm::vec3(0.0, 0.0, 0.0));
		modelCirculoMagico.render(matrixModelCirculo);

		matrixModelCirculo[3][1] = island2.getHeightTerrain(matrixModelCirculo[3][0], matrixModelCirculo[3][2]);
		matrixModelCirculo = glm::translate(matrixModelCirculo, glm::vec3(0.0, 150.0, 0.0));
		matrixModelCirculo = glm::rotate(matrixModelCirculo, 0.45f, glm::vec3(0, 0, 1));
		modelCirculoMagico.render(matrixModelCirculo);

		matrixModelCirculo[3][1] = island3.getHeightTerrain(matrixModelCirculo[3][0], matrixModelCirculo[3][2]);
		matrixModelCirculo = glm::translate(matrixModelCirculo, glm::vec3(0.0, 150.0, 0.0));
		//matrixModelCirculo = glm::rotate(matrixModelCirculo, 0.45f, glm::vec3(0, 0, 1));
		modelCirculoMagico.render(matrixModelCirculo);

		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		// Render lamp
		for(int i = 0; i < lamp1Position.size(); i++){
			lamp1Position[i].y = island1.getHeightTerrain(lamp1Position[i].x, lamp1Position[i].z);
			modelLamp1.setPosition(lamp1Position[i]);
			modelLamp1.setScale(glm::vec3(0.5));
			modelLamp1.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
			modelLamp1.render();
		}
		for(int i = 0; i < lamp2Position.size(); i++){
			lamp2Position[i].y = island1.getHeightTerrain(lamp2Position[i].x, lamp2Position[i].z);
			modelLamp2.setPosition(lamp2Position[i]);
			modelLamp2.setScale(glm::vec3(0.5));
			modelLamp2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
			modelLamp2.render();
			modelLampPost2.setPosition(lamp2Position[i]);
			modelLampPost2.setScale(glm::vec3(0.5));
			modelLampPost2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
			modelLampPost2.render();
		}
		
		/*****************************************
		 * Objetos animados por huesos
		 * **************************************/
		glm::mat4 modelMatrixKakashiBody = glm::mat4(modelMatrixKakashi);
		//Ajuste del eje y del objeto
		modelMatrixKakashiBody[3][1] = island1.getHeightTerrain(modelMatrixKakashiBody[3][0], modelMatrixKakashiBody[3][2]);
		switch (kakashiState) {
			case 0:
				modelKakashiQuieto.render(modelMatrixKakashiBody);
				break;
			case 1:
				modelKakashiCaminando.render(modelMatrixKakashiBody);
				break;
			case 2:
				modelKakashiCorriendo.render(modelMatrixKakashiBody);
				break;
			case 3:
				modelKakashiReversa.render(modelMatrixKakashiBody);
				break;
			default:
				break;
		}

		glm::mat4 modelMatrixKratosBody = glm::mat4(modelMatrixKratos);
		//Ajuste del eje y del objeto
		modelMatrixKratosBody[3][1] = island1.getHeightTerrain(modelMatrixKratosBody[3][0], modelMatrixKratosBody[3][2]);
		switch (kratosState) {
			case 0:
				modelKratosQuieto.render(modelMatrixKratosBody);
				break;
			case 1:
				modelKratosCaminando.render(modelMatrixKratosBody);
				break;
			case 2:
				modelKratosCorriendo.render(modelMatrixKratosBody);
				break;
			case 3:
				modelKratosReversa.render(modelMatrixKratosBody);
				break;
			default:
				break;
		}

		glm::mat4 modelMatrixNarutoBody = glm::mat4(modelMatrixNaruto);
		//Ajuste del eje y del objeto
		modelMatrixNarutoBody[3][1] = island1.getHeightTerrain(modelMatrixNarutoBody[3][0], modelMatrixNarutoBody[3][2]);
		switch (narutoState) {
			case 0:
				modelNarutoQuieto.render(modelMatrixNarutoBody);
				break;
			case 1:
				modelNarutoCaminando.render(modelMatrixNarutoBody);
				break;
			case 2:
				modelNarutoCorriendo.render(modelMatrixNarutoBody);
				break;
			case 3:
				modelNarutoReversa.render(modelMatrixNarutoBody);
				break;
			default:
				break;
		}

		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

void GenerarTextura(Texture texture, GLuint &textureID){
	// Definiendo la textura
	texture.loadImage(); // Cargar la textura
	glGenTextures(1, &textureID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(texture.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, texture.getChannels() == 3 ? GL_RGB : GL_RGBA, texture.getWidth(), texture.getHeight(), 0,
		texture.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texture.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	texture.freeImage(); // Liberamos memoria
}

void RenderTextura(GLuint Cesped, GLuint R, GLuint G, GLuint B, GLuint BlendMap){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Cesped);
			shaderTerrain.setInt("backgroundTexture", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, R);
			shaderTerrain.setInt("rTexture", 1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, G);
			shaderTerrain.setInt("gTexture", 2);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, B);
			shaderTerrain.setInt("bTexture", 3);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, BlendMap);
			shaderTerrain.setInt("blendMapTexture", 4);
			shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(80, 80)));
}