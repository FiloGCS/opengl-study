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
//stb_image library handles image loading
//This needs to be last since we're #including stb_image.h in previous includes
// and we can only implement it once!
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

	///GLFW and GLAD init
#pragma region Initialization

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

	///VIEWPORT
	// 1 and 2 set the origin position
	glViewport(0, 0, 800, 600);

	///MESH
#pragma region Mesh Definition
	// Generate a vertex array object (VAO)
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Mesh Data
	// Adding the vertices and indices of our triangle to a VBO and EBO
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
		0,1,3,
		1,2,3
	};

	// Generate a vertex buffer object (VBO)
	// glBufferData is a function specifically targeted to copy user-defined data into the currently bound buffer.
	//	We copy the previously defined vertex data into the GL_ARRAY_BUFFER buffer's memory
	//	The fourth parameter specifies how we want the graphics card to manage the given data. This can take 3 forms:
	//		GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times
	//		GL_STATIC_DRAW: the data is set only once and used many times.
	//		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	//	This is going to affect where the GPU will place the data and affect optimization.
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Generate an element buffer object (EBO)
	// Bind it as the current ELEMENT_ARRAY_BUFFER and fill it up
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//We need to tell OpenGL how it should interpret our vertex data.
	// Parameter Breakdown:
	// 1. Which vertex attribute we want to configure (in this case we specified location = 0 in the shader).
	// 2. Size of the vertex attribute (vec3 in this case, so 3 values).
	// 3. Type of data (vec3 are made of floats).
	// 4. Specifies if we want the data to be normalized.
	// 5. "Stride", the space between consecutive vertex attributes. (if the values are tightly packed, 0 also works)
	// 6. of type void*, this is the offset of where the position data begins in the buffer.
	// In this case: we're setting attribute of size 3 floats, without normalizing, tightly packed
	//We filled the info but we need to enable it so it's actually taken into account

	//Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//Color Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//UV Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

#pragma endregion

	///TEXTURES
	//From now on all textures will be loaded flipped vertically
	stbi_set_flip_vertically_on_load(true);
	//Texture 1
	Texture2D texture1("Textures/T_UV_04.jpg");
	//Texture 2
	Texture2D texture2("Textures/T_Noise_03a.png");
	//Bind texture 1 in the TextureUnit0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1.ID);
	//Bind texture 2 in the TextureUnit1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2.ID);

	///SHADER
	Shader myShader("06_usingTransform");
	myShader.use();
	//Assign the TextureUnits to the according shader uniforms
	myShader.setInt("u_texture1", 0);
	myShader.setInt("u_texture2", 1);




	///MATRIX TESTING

	//Translation transformation
	//glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	//glm::mat4 trans(1.0f);
	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	//vec = trans * vec;
	//std::cout << vec.x << vec.y << vec.z << std::endl;








	///LOOP
	//The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed to close.
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Scaling and rotating in realtime

		glm::mat4 trans = glm::mat4(1.0f);
		float t = glfwGetTime();
		trans = glm::translate(trans, glm::vec3(0, glm::sin(t) * 0.25f, 0));
		trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
		trans = glm::rotate(trans, glm::radians(t * 90.0f), glm::vec3(0.0, 0.0, 1.0));
		myShader.setMat4("transform", trans);


		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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