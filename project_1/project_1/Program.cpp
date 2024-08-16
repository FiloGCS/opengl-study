﻿#include <iostream>
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
//stb_image library handles image loading
//This needs to be last since we're #including stb_image.h in previous includes
// and we can only implement it once!
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.h"

//Global variables
//Window
ImVec4 clear_color = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
int window_width = 800;
int window_height = 600;
//Engine
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//Input - Mouse
float lastX = 400, lastY = 300;
bool firstMouse = true;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//ImGui windows
void drawImGuiWindow_settings(GLFWwindow* window, bool& show_demo_window);
void drawImGuiWindow_stats(GLFWwindow* window, float rendertime);

//Main
int main() {
	///// GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // would be needed in MacOSX
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenGL Program", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	///// ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	//ImGui state variables
	bool show_demo_window = false;

	///// GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	///// HERE WE GO!
	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST);

	//The list of entities in our scene
	std::vector<Entity> entities;

	//DEBUG - Populate with chad cubes
	int n = 0; // Or however many Entities you want
	entities.reserve(n);
	for (int i = 0; i < n; ++i) {
		entities.emplace_back();
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);
	//Loading a model
	char path[] = "Models/suzanne/suzanne_smooth.obj";
	//char path[] = "Models/backpack/backpack.obj";
	Model m1 = Model(path);
	Shader ourShader = Shader("default");

	/// START RENDER LOOP
	while (!glfwWindowShouldClose(window)) {
		//Update frame information
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Update Input
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//Generate ImGui windows
		if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}
		//drawImGuiWindow_settings(window, show_demo_window);

		//Clear buffer before starting rendering
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Update transform matrices
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -7.0f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / window_height, 0.1f, 100.0f);

		//Update each entity in the scene
		for (Entity& entity : entities) {
			entity.Update(glfwGetTime());
		}

		//Preparing OpenGL Query to measure render time
		GLuint queryID;
		glGenQueries(1, &queryID);

		//Render each entity in the scene, measuring with OpenGL queries
		glBeginQuery(GL_TIME_ELAPSED, queryID);
		for (Entity& entity : entities) {
			entity.Render(projection, view);
		}

		//TODO trying to make this work... T_T
		ourShader.use();
		//Engine uniforms
		ourShader.setFloat("time", glfwGetTime());
		//Lighting uniforms
		ourShader.setVector3("ambient_color", 0.1, 0.1, 0.15);
		ourShader.setVector3("point1_position", 3, 3, 2);
		ourShader.setVector3("point1_color", 1, 0.7, 0.7);
		ourShader.setFloat("point1_falloff", 5);

		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		//translation
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		//scale
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		//rotation
		float time = glfwGetTime();
		glm::vec3 rotSpeed = glm::vec3(0.0f, 25.0f, 0.0f);
		glm::quat rotation = glm::quat(glm::vec3(glm::radians(time * rotSpeed.x), glm::radians(time * rotSpeed.y), glm::radians(time * rotSpeed.z)));
		model = model * glm::mat4_cast(rotation);
		ourShader.setMat4("model", model);
		m1.Draw(ourShader);

		ImGui::Begin("Model info");
		std::ostringstream oss1;
		oss1 << "Vertices: " << m1.getVertexCount();
		ImGui::Text(oss1.str().c_str());
		std::ostringstream oss2;
		oss2 << "Meshes: " << m1.getMeshCount();
		ImGui::Text(oss2.str().c_str());
		ImGui::End();


		glEndQuery(GL_TIME_ELAPSED);
		GLuint64 elapsedTime;
		glGetQueryObjectui64v(queryID, GL_QUERY_RESULT, &elapsedTime);
		// Convert nanoseconds to microseconds
		float microseconds = elapsedTime / 1000.0f;
		//Show the result in the imGui window
		drawImGuiWindow_stats(window, microseconds);
		// Don't forget to delete the query object when you're done
		glDeleteQueries(1, &queryID);

		//Render ImGui windows on top of the scene
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Done!
		glfwSwapBuffers(window);
	}

	//Shutdown ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//Terminate GLFW
	glfwTerminate();

	return 0;
}

//Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//ImGui window functions
void drawImGuiWindow_settings(GLFWwindow* window, bool& show_demo_window) {
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");
	ImGui::Checkbox("Demo Window", &show_demo_window);
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	ImGui::SliderInt("Window Width", &window_width, 800, 1600);
	ImGui::SliderInt("Window Height", &window_height, 450, 900);
	glfwSetWindowSize(window, window_width, window_height);
	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	ImGui::End();
}
void drawImGuiWindow_stats(GLFWwindow* window, float rendertime) {
	ImGui::Begin("Stats");
	std::ostringstream oss;
	oss << "Render time: " << rendertime << "us";
	ImGui::Text(oss.str().c_str());
	ImGui::End();
}