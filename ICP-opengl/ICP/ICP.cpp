//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------

#include<iostream>
// OpenCV 
#include <opencv2\opencv.hpp>

// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> 

// GLFW toolkit
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shader.hpp"
#include"Maze.h"
#include"globals.h"
#include"Model.h"
#include"Mesh.h"
#include"Ghost.h"
#include"Light.h"

using namespace std;

Maze maze(15, 15);

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool fullscreen = false;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 960;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float yaw = 0;
float pitch = 0;

bool wPress = false;
bool aPress = false;
bool sPress = false;
bool dPress = false;

glm::vec3 ghostTarget(0, 0, 0);
glm::vec3 ghostDirection(1, 0, 0);

bool dirTurn = false;

bool eatGhost = false;

float color = 0.25;

void error_callback(int error, const char* description);
void fbsize_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {};

void initWindowedScreen();
void initFullScreen();

struct particle {
	glm::vec3 pos;
	glm::vec3 dir;
	float speed = 0.1f;
	float lifetime = 1.0f;
};

particle particles[100];
glm::vec3 particlePositions[100];
int particleToMove = 0;

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

static void initGlfw() {
	glfwSetErrorCallback(error_callback);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

static void initOpenGL(void)
{
	glfwMakeContextCurrent(globals.window);
	glewInit();
	glfwSetInputMode(globals.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glfwSetKeyCallback(globals.window, key_callback);
	glfwSetFramebufferSizeCallback(globals.window, fbsize_callback);
	glfwSetCursorPosCallback(globals.window, cursor_position_callback);

	glfwGetFramebufferSize(globals.window, &globals.width, &globals.height);
	glfwSwapInterval(1);


	fbsize_callback(globals.window, globals.width, globals.height);
}

float angleBetweenVectors(glm::vec2 v1, glm::vec2 v2) {
	return -atan2(v1.x * v2.x + v1.y * v2.y, v1.x * v2.y - v1.y * v2.x);
}

void changeGhostTarget(glm::vec3 ghostPos) {

	int x = (int)(ghostPos.x / 2 + 0.5f);
	int y = (int)(ghostPos.z / 2 + 0.5f);

	int dx = ghostDirection.x > 0.5f ? 1 : 0;
	dx = dx == 0 && ghostDirection.x < -0.5f ? -1 : dx;

	int dy = ghostDirection.z > 0.5f ? 1 : 0;
	dy = dy == 0 && ghostDirection.z < -0.5f ? -1 : dy;

	float X = round(ghostPos.x);
	float Z = round(ghostPos.z);

	int rnd = rand();

	dirTurn = rnd > RAND_MAX / 2;

	if (dirTurn) {
		int i = x - dy;
		int j = y - dx;

		if (i > 0 && i < maze.XSize && j > 0 && j < maze.YSize) {
			if (maze.maze[i][j] != '#') {
				ghostTarget = glm::vec3(X - dy * 2, ghostPos.y, Z - dx * 2);
				return;
			}
		}

		i = x + dy;
		j = y + dx;

		if (i > 0 && i < maze.XSize && j > 0 && j < maze.YSize) {
			if (maze.maze[i][j] != '#') {
				ghostTarget = glm::vec3(X + dy * 2, ghostPos.y, Z + dx * 2);
				return;
			}
		}
	}

	int i = x + dy;
	int j = y + dx;

	if (i > 0 && i < maze.XSize && j > 0 && j < maze.YSize) {
		if (maze.maze[i][j] != '#') {
			ghostTarget = glm::vec3(X + dy * 2, ghostPos.y, Z + dx * 2);
			return;
		}
	}

	i = x - dy;
	j = y - dx;

	if (i > 0 && i < maze.XSize && j > 0 && j < maze.YSize) {
		if (maze.maze[i][j] != '#') {
			ghostTarget = glm::vec3(X - dy * 2, ghostPos.y, Z - dx * 2);
			return;
		}
	}

	i = x + dx;
	j = y + dy;

	if (i > 0 && i < maze.XSize && j > 0 && j < maze.YSize) {
		if (maze.maze[i][j] != '#') {
			ghostTarget = glm::vec3(X + dx * 2, ghostPos.y, Z + dy * 2);
			return;
		}
	}

	i = x - dx;
	j = y - dy;

	if (i > 0 && i < maze.XSize && j > 0 && j < maze.YSize) {
		if (maze.maze[i][j] != '#') {
			ghostTarget = glm::vec3(X - dx * 2, ghostPos.y, Z - dy * 2);
			return;
		}
	}
}

bool collision() {

	float X = cameraPos.x;
	float Y = cameraPos.z;

	int x = (int)(X / 2 + 0.5f);
	int y = (int)(Y / 2 + 0.5f);

	float radius2 = 0.25f;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			int indexX = x + i;
			int indexY = y + j;
			if (indexX > -1 && indexY > -1 && indexX < maze.XSize && indexY < maze.YSize) {
				if (maze.maze[indexX][indexY] == '#') {
					float x1 = indexX * 2 - 1;
					float x2 = indexX * 2 + 1;
					float y1 = indexY * 2 - 1;
					float y2 = indexY * 2 + 1;

					float d1 = X - x1;
					float d2 = X - x2;
					float d3 = Y - y1;
					float d4 = Y - y2;

					d1 *= d1;
					d2 *= d2;
					d3 *= d3;
					d4 *= d4;
					
					if (radius2 >= d1 + d3) {
						return true;
					}
					if (radius2 >= d1 + d4) {
						return true;
					}
					if (radius2 >= d2 + d3) {
						return true;
					}
					if (radius2 >= d2 + d4) {
						return true;
					}

					if (X < x2 && X > x1) {
						if (Y > y2 && Y - y2 <= 0.5f) {
							return true;
						}
						if (Y < y1 && y1 - Y <= 0.5f) {
							return true;
						}
					}

					if (Y < y2 && Y > y1) {
						if (X > x2 && X - x2 <= 0.5f) {
							return true;
						}
						if (X < x1 && x1 - X <= 0.5f) {
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void respawnGhost(Ghost& ghost) {
	for (int i = 0; i < maze.XSize; i++)
	{
		for (int j = 0; j < maze.YSize; j++)
		{
			if (maze.maze[i][j] != '#') {
				int dx = i - cameraPos.x / 2;
				int dy = j - cameraPos.z / 2;

				if (dx * dx + dy * dy > 50) {
					ghost.position = glm::vec3(i * 2, -0.5f, j * 2);
					ghost.changeColor(color);
					color += 0.25;
					if (color > 0.76f) {
						color = 0.0f;
					}
					return;
				}
			}
		}
	}
}

void changeLightPos(Light& light) {
	srand(time(NULL));
	int i = rand() / ((RAND_MAX + 1u) / maze.XSize);
	int j = rand() / ((RAND_MAX + 1u) / maze.YSize);
	while (maze.maze[i][j] == '#') {
		i = rand() / ((RAND_MAX + 1u) / maze.XSize);
		j = rand() / ((RAND_MAX + 1u) / maze.YSize);
	}
	light.resetModelMatrix();
	light.translate(glm::vec3(i * 2, -0.5f, j * 2));
}

int main()
{
	initWindowedScreen();

	GLuint shaderID = LoadShaders("resources/vertexShader.vert", "resources/fragmentShader.frag");
	GLuint lightShaderID = LoadShaders("resources/light.vert", "resources/light.frag");
	GLuint particleShaderID = LoadShaders("resources/particle.vert", "resources/particle.frag");

	Texture ghostTexture("resources/ghostTexture.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_BGR, GL_UNSIGNED_BYTE);
	Texture wallTexture("resources/wallTexture.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_BGR, GL_UNSIGNED_BYTE);
	Texture pacmanTexture("resources/pacMan.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_BGR, GL_UNSIGNED_BYTE);

	Mesh pacmanMesh("resources/pacman.obj");
	Mesh wallMesh("resources/wall.obj");

	Mesh bodyMesh("resources/ghost.obj");
	Mesh eyeMesh("resources/eye.obj");
	Mesh pupilMesh("resources/pupil.obj");

	Ghost ghost(bodyMesh, eyeMesh, pupilMesh, ghostTexture, shaderID);

	Model pacman(pacmanMesh, pacmanTexture, shaderID);
	Model floor(wallMesh, wallTexture, shaderID);
	Light light(eyeMesh, glm::vec3(1, 1, 0.7f), lightShaderID);

	for (int i = 0; i < 100; i++)
	{
		particles[i] = particle{
			glm::vec3(0),
			glm::normalize(glm::vec3(rand() - RAND_MAX / 2,rand() - RAND_MAX / 2,rand() - RAND_MAX / 2))
		};
	}

	GLuint partID;

	glGenBuffers(1, &partID);
	glBindBuffer(GL_ARRAY_BUFFER, partID);
	glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::vec3), &particlePositions[0], GL_STATIC_DRAW);

	glUseProgram(shaderID);
	glUniform3f(glGetUniformLocation(shaderID, "lightDirection"), 1.0f, 1.0f, 0.1f);
	glUniform3f(glGetUniformLocation(shaderID, "lightColor"), light.color.r, light.color.g, light.color.b);
	glUniform1i(glGetUniformLocation(shaderID, "myTextureSampler"), 0);

	glUseProgram(particleShaderID);
	glUniform4f(glGetUniformLocation(particleShaderID, "particleColor"), light.color.r, light.color.g, light.color.b, 1);

	cameraPos = glm::vec3(maze.StartX * 2, -0.5f, maze.StartY * 2);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	std::vector<Model> walls;
	bool translated = false;
	floor.translate(glm::vec3(maze.XSize, -2, maze.YSize));
	floor.scale(glm::vec3(maze.XSize * 1.5f, 1, maze.YSize * 1.5f));
	for (int i = 0; i < maze.XSize; i++)
	{
		for (int j = 0; j < maze.YSize; j++)
		{
			if (maze.maze[i][j] == '#') {
				Model newModel(wallMesh, wallTexture, shaderID);
				newModel.translate(glm::vec3(i * 2, 0, j * 2));
				walls.push_back(newModel);
			}
			else {
				int dx = i - maze.StartX;
				int dy = j - maze.StartY;

				if (dx * dx + dy * dy > 100) {
					ghost.position = glm::vec3(i * 2, -0.5f, j * 2);
				}
			}
		}
	}
	float pAngle = 0;
	ghost.scale(glm::vec3(0.5f, 0.5f, 0.5f));
	glm::vec3 gp = ghost.getWorldPos();
	changeGhostTarget(gp);
	light.translate(glm::vec3(2, -0.5f, 4));
	float eatGhostTime = 0.0f;

	while (!glfwWindowShouldClose(globals.window))
	{
		float currentFrame = glfwGetTime();
		if (currentFrame - eatGhostTime > 15.0f) {
			eatGhost = false;
		}
		if (particleToMove < 100) {
			particleToMove++;
		}
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		float moveSpeed = std::min(2.5f * deltaTime,0.1f);
		glm::vec3 cf = glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
		glm::vec3 newPos = cameraPos;

		if (wPress)
			newPos += moveSpeed * cf;
		if (sPress)
			newPos -= moveSpeed * cf;
		if (aPress)
			newPos -= glm::normalize(glm::cross(cf, cameraUp)) * moveSpeed;
		if (dPress)
			newPos += glm::normalize(glm::cross(cf, cameraUp)) * moveSpeed;

		float dx = newPos.x - cameraPos.x;
		float dy = newPos.z - cameraPos.z;
		glm::vec3 oldPos = cameraPos;
		cameraPos.x = newPos.x;
		if (collision()) {
			cameraPos.x = oldPos.x;
		}
		cameraPos.z = newPos.z;
		if (collision()) {
			cameraPos.z = oldPos.z;
		}

		pacman.resetModelMatrix();
		pacman.translate(cameraPos);
		if ((dx * dx + dy * dy > 0.00001)) {
			pAngle = angleBetweenVectors(glm::vec2(dx, dy), glm::vec2(0, -1));
		}
		pacman.rotate(pAngle, glm::vec3(0, 1, 0));
		pacman.scale(glm::vec3(0.5f, 0.5f, 0.5f));

		glm::vec3 lightPos = light.getWorldPos();

		dx = cameraPos.x - lightPos.x;
		dy = cameraPos.z - lightPos.z;

		if (dx * dx + dy * dy < 0.25f) {
			eatGhost = true;
			eatGhostTime = glfwGetTime();
			changeLightPos(light);
		}

		ghost.translate(ghostDirection * moveSpeed);

		glm::vec3 ghostPos = ghost.getWorldPos();

		dx = cameraPos.x - ghostPos.x;
		dy = cameraPos.z - ghostPos.z;

		if (dx * dx + dy * dy < 0.5f) {
			if (eatGhost) {
				respawnGhost(ghost);
				ghostPos = ghost.getWorldPos();
				changeGhostTarget(ghostPos);
				ghostDirection = glm::normalize(glm::vec3(ghostTarget.x - ghostPos.x, 0, ghostTarget.z - ghostPos.z));
				ghost.angle = angleBetweenVectors(glm::vec2(ghostDirection.x, ghostDirection.z), glm::vec2(1, 0));;
			}
			else {
				cameraPos = glm::vec3(maze.StartX * 2, -0.5f, maze.StartY * 2);
			}
		}

 		dx = ghostTarget.x - ghostPos.x;
		dy = ghostTarget.z - ghostPos.z;

		if (dx * dx + dy * dy < 0.01f) {
			changeGhostTarget(ghostPos);
			ghostDirection = glm::normalize(glm::vec3(ghostTarget.x - ghostPos.x, 0, ghostTarget.z - ghostPos.z));
			ghost.angle = angleBetweenVectors(glm::vec2(ghostDirection.x, ghostDirection.z), glm::vec2(1, 0));
		}
		ghostDirection = glm::normalize(glm::vec3(ghostTarget.x - ghostPos.x, 0, ghostTarget.z - ghostPos.z));

		newPos = cameraPos - cameraFront * glm::vec3(2, 2, 2);

		glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = glm::lookAt(newPos, cameraPos, cameraUp);
		projection = glm::perspective(glm::radians(60.0f), (float)globals.width / globals.height, 0.1f, 100.0f);

		glm::mat4 vp = projection * view;

		glUseProgram(particleShaderID);
		glUniform3f(glGetUniformLocation(particleShaderID, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(glGetUniformLocation(particleShaderID, "VP"), 1, GL_FALSE, &vp[0][0]);
		for (int i = 0; i < particleToMove; i++)
		{
			glm::vec3 moveVec = particles[i].dir * glm::vec3(particles[i].speed * 0.1f);
			particles[i].pos += moveVec;
			particlePositions[i] = lightPos + particles[i].pos;
			particles[i].lifetime -= deltaTime;
			if (particles[i].lifetime <= 0.0f) {
				particles[i].pos = glm::vec3(0);
				particles[i].lifetime = 1.0f;
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, partID);
		glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::vec3), particlePositions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_POINTS, 0, 100);
		glDisableVertexAttribArray(0);

		glUseProgram(lightShaderID);
		light.draw(vp);

		glUseProgram(shaderID);
		glUniform3f(glGetUniformLocation(shaderID, "camPos"), newPos.x, newPos.y, newPos.z);
		glUniform3f(glGetUniformLocation(shaderID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		floor.draw(vp);
		pacman.draw(vp);
		for (int i = 0; i < walls.size(); i++)
		{
			walls[i].draw(vp);
		}

		glEnable(GL_BLEND);
		ghost.draw(vp, cameraPos);
		glDisable(GL_BLEND);

		glfwSwapBuffers(globals.window);
		glfwPollEvents();
	}

	ghostTexture.Delete();
	wallTexture.Delete();
	pacmanTexture.Delete();

	glDeleteProgram(shaderID);
	glDeleteProgram(lightShaderID);

	glfwDestroyWindow(globals.window);
	glfwTerminate();
	return 0;
}

static void initFullScreen() {
	glfwInit();
	int srcWidth;
	int srcHeight;
	GetDesktopResolution(srcWidth, srcHeight);
	globals.window = glfwCreateWindow(srcWidth, srcHeight, "OpenGL context", glfwGetPrimaryMonitor(), NULL);
	initOpenGL();
}

static void initWindowedScreen() {
	glfwInit();
	globals.window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL context", NULL, NULL);
	initOpenGL();
}

void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		yaw = -90.0f;
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		return;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.2f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 0.0f)
		pitch = 0.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		fullscreen = !fullscreen;
		int scrWidth = SCR_WIDTH;
		int scrHeight = SCR_HEIGHT;
		if (fullscreen) {
			GetDesktopResolution(scrWidth, scrHeight);
		}
		glfwSetWindowMonitor(window, fullscreen ? glfwGetPrimaryMonitor() : NULL, 0, 0, scrWidth, scrHeight, GLFW_DONT_CARE);
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		wPress = true;
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		aPress = true;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		sPress = true;
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		dPress = true;
	}
	else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		wPress = false;
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		aPress = false;
	}
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		sPress = false;
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		dPress = false;
	}
}

void fbsize_callback(GLFWwindow* window, int width, int height)
{
	if (height == 0)
		height = 1;

	float ratio = width * 1.0f / height;
	globals.width = width;
	globals.height = height;

	projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 20000.0f);

	glViewport(0, 0, width, height);

}