//Following this course: https://learnopengl.com/Getting-started/Hello-Window

//How to hide the console in Visual Studio
// Properties->Linker->System->SubSystem (set it to Windows)
// Properties->Linker->Advanced->Entry Point (set it to mainCRTStartup)
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>
//GLFW for window control and glad for OpenGL functions
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//My includes
#include "Shader.h"
#include "Texture2D.h"
#include "RenderObject.h"
//stb_image library handles image loading
//This needs to be last since we're #including stb_image.h in previous includes
// and we can only implement it once!
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

#pragma region GLFW_and_GLAD_Initialization

	glfwInit();
	//With these functions we configure glfw.
	// The first attribute tells what option we want to configure
	// Reference: https://www.glfw.org/docs/latest/window.html#window_hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Next option would be needed in MacOSX
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//Create a window with GLFW
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//Make the context of our window the main context on the current thread
	glfwMakeContextCurrent(window);

	//GLAD manages function pointers for OpenGL, so we initialize GLAD before calling any OpenGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	//We can register a callback function on the window that gets called each time the window is resized.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
#pragma endregion

	//Create a viewport
	glViewport(0, 0, 800, 600);

	//Create a cube object
	Entity myCube = Entity();

	//Enable depth Test
	glEnable(GL_DEPTH_TEST);

	///RENDER LOOP
	//The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed to close.
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		///MATRIX TRANSFORMATIONS
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600, 0.1f, 100.0f);
	
		//Update Objects
		myCube.Update(glfwGetTime());
		//Render RenderObjects
		myCube.Render(projection, view);

		glfwSwapBuffers(window);
		//The glfwPollEvents function checks if any events are triggered
		// (like keyboard input or mouse movement events),
		// updates the window state, and calls the corresponding functions 
		// (which we can register via callback methods).
		glfwPollEvents();
	}

	///TERMINATION
	//As soon as we exit the render loop we properly clean/delete all of GLFW's resources.
	glfwTerminate();
	return 0;
}