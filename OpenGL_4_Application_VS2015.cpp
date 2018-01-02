//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"


std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float movementSpeed = 1.0f;
glm::vec3 fogFactor;
int glWindowWidth = 1500;
int glWindowHeight = 1000;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLfloat numberOfLights = 1.0f;
// pozitii lumini
glm::vec3 LightPositions[] = {
	glm::vec3(18.1194f,	6.5f,  12.53555f)

};


glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;
glm::mat4 modelUfo;
GLuint modelLocUfo;
glm::mat4 viewUof;
GLuint viewLocUfo;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightDirPoint;
GLuint lightDirPointLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec3 lightColorPoint;
GLuint lightColorPointLoc;

gps::Camera *myCamera = new gps::Camera(glm::vec3(10.0f, 10.0f, 2.5f), glm::vec3(0.0f, 10.0f, 10.0f));

//sun coords
float sunX = 77.0f;
float sunY = 110.0f;
float sunZ = -49.0f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;

gps::Model3D myModel;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;
gps::Model3D ufo;
gps::Shader ufoShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	lightShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_Q]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_E]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_I]) {
		myCustomShader.useShaderProgram();
		lightDir = glm::vec3(sunX, sunY, sunZ++);
		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
	}

	if (pressedKeys[GLFW_KEY_K]) {
		myCustomShader.useShaderProgram();
		lightDir = glm::vec3(sunX, sunY, sunZ--);
		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
	}

	if (pressedKeys[GLFW_KEY_U]) {
		myCustomShader.useShaderProgram();
		lightDir = glm::vec3(sunX++, sunY, sunZ);
		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	}

	if (pressedKeys[GLFW_KEY_J]) {
		myCustomShader.useShaderProgram();
		lightDir = glm::vec3(sunX--, sunY, sunZ);
		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	}

	if (pressedKeys[GLFW_KEY_O]) {
		myCustomShader.useShaderProgram();
		lightDir = glm::vec3(sunX, sunY++, sunZ);
		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	}

	if (pressedKeys[GLFW_KEY_L]) {
		myCustomShader.useShaderProgram();
		lightDir = glm::vec3(sunX, sunY--, sunZ);
		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = -1.0f, far_plane = 500.0f;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDir, myCamera->getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void loadSkybox() {
	faces.push_back("textures/skybox/right.tga");
	faces.push_back("textures/skybox/left.tga");
	faces.push_back("textures/skybox/top.tga");
	faces.push_back("textures/skybox/bottom.tga");
	faces.push_back("textures/skybox/back.tga");
	faces.push_back("textures/skybox/front.tga");
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera->getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
}

void initModels()
{ 
	myModel = gps::Model3D("objects/modelScene.obj", "objects/");
	lightCube = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
	ufo = gps::Model3D("objects/UFO & texture/untitled.obj", "objects/UFO & texture/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
	ufoShader.loadShader("shaders/ufo.vert", "shaders/ufo.frag");
	
}

void initUniforms()	
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "cameraPosEye"), 1, glm::value_ptr(myCamera->getCameraPosition()));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "nbOfPointLights"), 1, &numberOfLights);
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "fogDensity"), 1, GL_FALSE, glm::value_ptr(fogFactor));
	//lumina soare
	lightDir = glm::vec3(sunX, sunY, sunZ);//77 max 110.0f -49.0f min279x
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//culoare
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	myCustomShader.useShaderProgram();
	//pentru celelalte lumini punctiforme
	//for (int i=0; i < numberOfLights; ++i) {
		lightDirPoint = LightPositions[0];
		lightDirPointLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointPos");
		glUniform3fv(lightDirPointLoc, 1, glm::value_ptr(lightDirPoint));

		//set light color
		lightColorPoint = glm::vec3(1.0f, 0.75f, 0.25f); //galben
		lightColorPointLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColorPoint");
		glUniform3fv(lightColorPointLoc, 1, glm::value_ptr(lightColorPoint));

		lightShader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	//}

	ufoShader.useShaderProgram();

	modelLocUfo = glGetUniformLocation(ufoShader.shaderProgram, "model");
	
	viewLocUfo = glGetUniformLocation(ufoShader.shaderProgram, "view");
	glUniform3fv(viewLocUfo, 1, glm::value_ptr(lightDir));
	projectionLoc = glGetUniformLocation(ufoShader.shaderProgram, "projection");
	glUniform3fv(glGetUniformLocation(ufoShader.shaderProgram, "lightDir") , 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

}
float delta = 77.0f;
float fogFactorScale = 0.01f;
float unghiUfo = 0.0f;
glm::vec3 direction;

void renderScene()
{

	processMovement();	

	//render the scene to the depth buffer (first pass)
	
	depthMapShader.useShaderProgram();
	glCullFace(GL_FRONT);
	
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
		
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));	
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	myModel.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
	//render the scene (second pass)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera->getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));	

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	
	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	myModel.Draw(myCustomShader);
		
	//draw a white cube around the light


	direction.y = lightDir.y;
	direction.z = lightDir.z;
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	if (deltaTime > 0.037f) {//27hz refresh frate.
		myCustomShader.useShaderProgram();
		lightDir = direction;
		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
	
		model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, direction);
		model = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		if (delta < -290.0f || delta > 100.0f) {
			movementSpeed = -movementSpeed;
		}
		printf("%f\n", fogFactor.x);
		if (fogFactor.x > 0.1f || fogFactor.x < 0.0f) {
			fogFactorScale = -fogFactorScale;
		}
		fogFactor.x += fogFactorScale/25;
		unghiUfo += 0.1f;
		delta  -= movementSpeed;
		direction.x = delta;
		lastFrame = currentFrame;

		printf("A a X=%f, Y=%f, Z=%f\n", direction.x, direction.y, direction.z);
		lightCube.Draw(lightShader);
	}
	
	lightShader.useShaderProgram();
	//for (int i = 0; i < numberOfLights; ++i) {
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, LightPositions[0]);
	model = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//printf("%d\n", i);
	//printf("B b X=%f, Y=%f, Z=%f\n", LightPositions[0].x, LightPositions[0].y, LightPositions[0].z);
	lightCube.Draw(lightShader);
	//}


	//culoare
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	//ptr fog
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity"), 1, GL_FALSE, glm::value_ptr(fogFactor));
	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


	//ufos

	ufoShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(ufoShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(glGetUniformLocation(ufoShader.shaderProgram, "lightDirMatrix"), 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glUniformMatrix4fv(glGetUniformLocation(ufoShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	modelUfo = glm::rotate(glm::mat4(1.0f), glm::radians(unghiUfo), glm::vec3(0.0f, 1.0f, 0.0f));
	modelUfo = glm::translate(modelUfo, glm::vec3(100.0f, 100.0f, -50.0f));
	modelUfo = glm::scale(modelUfo, glm::vec3(1.05f, 1.05f, 1.05f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelUfo));
	ufo.Draw(ufoShader);

	mySkyBox.Draw(skyboxShader, view, projection);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	myCamera->keyboardCallback(window, key, scancode, action, mode);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	myCamera->mouseCallback(window, xpos, ypos);
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	loadSkybox();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();	
	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		
		renderScene();
		glfwSetKeyCallback(glWindow, keyboardCallback);
		glfwSetCursorPosCallback(glWindow, mouseCallback);
		glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
