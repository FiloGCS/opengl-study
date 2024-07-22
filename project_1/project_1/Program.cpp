#include <iostream>
#include <vector>
//GLFW for window control and glad for OpenGL functions
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//imgui to handle Debug GUI
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
//My includes
#include "Shader.h"
#include "Texture2D.h"
#include "Entity.h"
#include "Camera.cpp"
//stb_image library handles image loading
//This needs to be last since we're #including stb_image.h in previous includes
// and we can only implement it once!
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Global variables
ImVec4 clear_color = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
int window_width = 800;
int window_height = 600;
// Global camera object
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// Variables for mouse handling
float lastX = 400, lastY = 300;
bool firstMouse = true;
// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


int main() {
	//-----INIT GLFW
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
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenGL Program", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//Make the context of our window the main context on the current thread
	glfwMakeContextCurrent(window);

	//-----INIT imgui
	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	//Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
	//ImGui state variables
    bool show_demo_window = false;



	//-----INIT GLAD
	//GLAD manages function pointers for OpenGL, so we initialize GLAD before calling any OpenGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	//We can register a callback function on the window that gets called each time the window is resized.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//Set the mouse and keyboard callback
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	 
	//Create a viewport
	glViewport(0, 0, window_width, window_height);
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

		// Per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Input
		processInput(window);


		//The glfwPollEvents function checks if any events are triggered
		// (like keyboard input or mouse movement events),
		// updates the window state, and calls the corresponding functions 
		// (which we can register via callback methods).
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//If desired, show demo window
		if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}
		//Create our own ImGui window
#pragma region Test_ImGui_window
		static float f = 0.0f;
		static int counter = 0;
		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Hello, world!");
		// Display some text (you can use a format strings too)
		ImGui::Text("This is some useful text.");
		// Edit bools storing our window open/close state
		ImGui::Checkbox("Demo Window", &show_demo_window);
		// Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::SliderInt("Window Width", &window_width, 800, 1600);
		ImGui::SliderInt("Window Height", &window_height, 450, 900);
		glfwSetWindowSize(window, window_width, window_height);
		// Edit 3 floats representing a color
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::End();
#pragma endregion




		//Clear Buffers
		glClearColor(clear_color.x , clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//MATRIX TRANSFORMATIONS
		glm::mat4 view = camera.GetViewMatrix();
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), ((float)window_width/(float)window_height), 0.1f, 100.0f);

		//Update and render entities
		for (Entity& entity : entities) {
			entity.Update(glfwGetTime());
			entity.Render(projection, view); // Call the Render method on each Entity
		}

		//Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Swap Buffers
		glfwSwapBuffers(window);
	}

	//TERMINATION
	//As soon as we exit the render loop we properly clean/delete all of GLFW's resources.
	glfwTerminate();
	//Shutdown imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}