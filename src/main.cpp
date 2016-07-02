/*
*	MIT License
*
* Copyright (c) <2016> <Cristian Rodríguez Bernal (maldicion069)>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "SimpleScene.h"
#include "IScene.h"
#include <string>
#include "Camera.h"
#include <sstream>

using namespace std; 

Camera camera(glm::vec3(-5.0f, 2.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -1.75f, -5.75f);
GLfloat lastX = WINDOW_WIDTH / 2.0;
GLfloat lastY = WINDOW_HEIGHT / 2.0;
bool    keys[1024];

SimpleScene *scene;
GLFWwindow * window;
std::string title;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false; 
	}
}

void do_movement() {
	float speed = 1.0f;
	if (keys[GLFW_KEY_LEFT_SHIFT]) {
		speed = 5.0f;
	}
	// Camera controls
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime * speed);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime * speed);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime * speed);
	if (keys[GLFW_KEY_Z])
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (keys[GLFW_KEY_X])
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (keys[GLFW_KEY_O])
		scene->showQuad(false);
	if (keys[GLFW_KEY_P])
		scene->showQuad(true);
	if (keys[GLFW_KEY_L])
		scene->showSolid(false);
	if (keys[GLFW_KEY_F])
		scene->showSolid(true);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mainLoop() {
	const int samples = 60;
	float time[samples], currentFrame;
	int index = 0; 
	glfwSwapInterval(1);

	std::cout << "LSHIFT\t: Increment camera movement" << std::endl;
	std::cout << "W\t: Forward camera" << std::endl;
	std::cout << "A\t: Left camera" << std::endl;
	std::cout << "D\t: Right camera" << std::endl;
	std::cout << "S\t: Backward camera" << std::endl;
	std::cout << "P\t: Textured quad" << std::endl;
	std::cout << "O\t: Non textured quad" << std::endl;
	std::cout << "L\t: Line quad" << std::endl;
	std::cout << "F\t: Solid quad" << std::endl;

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		do_movement();

		scene->update(float(glfwGetTime()));
		scene->draw(&camera);

		glfwSwapBuffers(window);

		// Update FPS
		time[index] = float(glfwGetTime());
		index = (index + 1) % samples;

		if( index == 0 ) {
			float sum = 0.0f;
			for( int i = 0; i < samples-1 ; i++ )
				sum += time[i + 1] - time[i];
			float fps = samples / sum;

			stringstream strm;
			strm << title;
			strm.precision(4);
			strm << " [FPS: " << fps << "]";
			glfwSetWindowTitle(window, strm.str().c_str());
		}
	}
}

void resizeGL(int w, int h ) {
	scene->resize(w,h);
}

void window_size_callback(GLFWwindow* window, int w, int h) {
	camera.screenWidth = w;
	camera.screenHeight = h;
	resizeGL(w, h);
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLU_TRUE);

	title = "";
	window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, title.c_str(), NULL, NULL );
	
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	scene = new SimpleScene(WINDOW_WIDTH, WINDOW_HEIGHT);
	scene->initScene();

	camera.screenWidth = WINDOW_WIDTH;
	camera.screenHeight = WINDOW_HEIGHT;

	resizeGL(WINDOW_WIDTH, WINDOW_HEIGHT);

	mainLoop();

	glfwTerminate();
	return 0;
}