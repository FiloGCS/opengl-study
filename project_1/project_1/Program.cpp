#include <iostream>
#include <vector>
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

constexpr glm::vec3 CLEAR_COLOR = glm::vec3(0.15, 0.16, 0.13);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

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
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Program", NULL, NULL);
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
	 
	//Create a viewport
	glViewport(0, 0, 800, 600);
	//Enable depth Test
	glEnable(GL_DEPTH_TEST);

	std::vector<Entity> entities;
	
	int n = 1; // Or however many Entities you want
	entities.reserve(n); // Reserve memory for n entities (optional for performance)
	for (int i = 0; i < n; ++i) {
		entities.emplace_back(); // Constructs a new Entity in place
	}
	//Create a cube object
	Entity myCube = Entity();

	///RENDER LOOP
	//The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed to close.
	while (!glfwWindowShouldClose(window)){

		glClearColor(CLEAR_COLOR.x, CLEAR_COLOR.y, CLEAR_COLOR.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//MATRIX TRANSFORMATIONS
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600, 0.1f, 100.0f);
	
		//Update and render Objects
		for (Entity& entity : entities) {
			entity.Update(glfwGetTime());
			for (int i = 0; i < 1000; i++) {
				entity.Render(projection, view); // Call the Render method on each Entity
			}
		}

		glfwSwapBuffers(window);
		//The glfwPollEvents function checks if any events are triggered
		// (like keyboard input or mouse movement events),
		// updates the window state, and calls the corresponding functions 
		// (which we can register via callback methods).
		glfwPollEvents();
	}

	//TERMINATION
	//As soon as we exit the render loop we properly clean/delete all of GLFW's resources.
	glfwTerminate();
	return 0;
}