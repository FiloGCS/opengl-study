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
//stb_image library handles image loading
//This needs to be last since we're #including stb_image.h in previous includes
// and we can only implement it once!
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.h"
#include "UtilsNumbers.h"

//GLOBAL VARIABLES-------------------------
//Window
ImVec4 clear_color = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
int window_width = 1280;
int window_height = 720;
//Engine
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//Lighting
glm::vec3 ambient_color = glm::vec3(0.1, 0.1, 0.15);
glm::vec3 point1_color = glm::vec3(1, 0.95, 0.8);
glm::vec3 point1_position = glm::vec3(1, 1, 3);
float point1_falloff = 5;
//Interface
float nextStatsUpdateTime = 0;
float statsUpdateFreq = 4; //Times to update stats (per second)
float renderTime = 0;
std::vector<Shader*> loadedShaders;
static int selectedShader = 0;
//Input - Mouse
float lastX = 400, lastY = 300;
bool firstMouse = true;

//FUNCTION PROTOTYPES-----------------------
//callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//ImGui window functions
void drawImGuiWindow_settings(GLFWwindow* window, bool& show_demo_window);
void drawImGuiWindow_stats(GLFWwindow* window, float rendertime);
void drawImGuiWindow_environment(GLFWwindow* window, glm::vec3& ambient, glm::vec3& point1, float&point1_falloff);
void drawImGuiWindow_modelInfo(Model m);

//MAIN ----------------------------------------------------------------------------
int main() {
	//GLFW------------------------------
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

	//ImGUI-----------------------------
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

	//GLAD------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//HERE WE GO!-----------------------
	glViewport(0, 0, window_width, window_height);

	//STENCIL BUFFER
	//glEnable(GL_STENCIL_TEST);
	//glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFF); // each bit is written to the stencil buffer as is
	//glStencilMask(0x00); // each bit ends up as 0 in the stencil buffer (disabling writes)
	//glStencilFunc describes the test
	//If stencil value of a fragment is equal to ref value of 0, passes test and is drawn:
	glStencilFunc(GL_EQUAL, 0, 0xFF);
	//glStencilOp describes what actions to take
	//in order:
	//sfail: action to take if the stencil test fails.
	//dpfail : action to take if the stencil test passes, but the depth test fails.
	//dppass : action to take if both the stencil and the depth test pass.
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	//DEPTH BUFFER
	glEnable(GL_DEPTH_TEST);
	//This is the Depth testing functions
	//it passes if fragment depth is "GL_LESS" than the buffer
	//Other functions: GL_ALWAYS, GL_NEVER, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL
	glDepthFunc(GL_LESS);	
	//this can be used to temporarily disable WRITING while still testing
	//glDepthMask(GL_FALSE);

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


	Shader shader1 = Shader("default");
	loadedShaders.push_back(&shader1);
	Shader shader2 = Shader("default_UV");
	loadedShaders.push_back(&shader2);
	Shader shader3 = Shader("default_Normal");
	loadedShaders.push_back(&shader3);


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

		//Clear buffers before starting rendering
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Update transform matrices
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
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

		Shader* shader = loadedShaders.at(selectedShader);
		//TODO trying to make this work... T_T
		shader->use();
		//Engine uniforms
		shader->setVector2("resolution", window_width, window_height);
		shader->setFloat("time", glfwGetTime());
		drawImGuiWindow_environment(window, ambient_color, point1_color, point1_falloff);
		//Lighting uniforms
		shader->setVector3("ambient_color", ambient_color);
		shader->setVector3("point1_position", point1_position);
		shader->setVector3("point1_color", point1_color);
		shader->setFloat("point1_falloff", point1_falloff);

		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
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
		shader->setMat4("model", model);
		m1.Draw(*shader);

		drawImGuiWindow_modelInfo(m1);


		glEndQuery(GL_TIME_ELAPSED);
		GLuint64 elapsedTime;
		glGetQueryObjectui64v(queryID, GL_QUERY_RESULT, &elapsedTime);
		// Convert nanoseconds to microseconds
		float microseconds = elapsedTime / 1000.0f;
		if (nextStatsUpdateTime <= glfwGetTime()) {
			renderTime = (float)(int)microseconds;
			nextStatsUpdateTime = glfwGetTime() + (1 / statsUpdateFreq);
		}
		//Show the result in the imGui window
		drawImGuiWindow_stats(window, renderTime);
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
	ImGui::SliderInt("Window Width", &window_width, 800, 1920);
	ImGui::SliderInt("Window Height", &window_height, 450, 1080);
	glfwSetWindowSize(window, window_width, window_height);
	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	ImGui::End();
}
void drawImGuiWindow_stats(GLFWwindow* window, float rendertime) {
	ImGui::Begin("Stats");
	std::ostringstream oss;
	oss << "Render time: " << rendertime << " microseconds";
	ImGui::Text(oss.str().c_str());
	ImGui::End();
}
void drawImGuiWindow_environment(GLFWwindow* window, glm::vec3& ambient, glm::vec3&point1, float& point1_falloff) {

	ImGui::Begin("Environment");
	//const char* items[] = { "default", "UV_CHECK"};
	//ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
	ImGui::InputInt(std::to_string(loadedShaders.at(selectedShader%loadedShaders.size())->ID).c_str(), &selectedShader);
	selectedShader = selectedShader % loadedShaders.size();
	ImGui::Text("Environment");
	//TODO - Reset button
	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	ImGui::ColorEdit3("ambient light", (float*) &ambient);
	ImGui::Text("Point Light 1");
	ImGui::InputFloat3("position", &point1_position.x);
	ImGui::ColorEdit3("color", (float*) &point1);
	ImGui::SliderFloat("falloff", &point1_falloff, 0.25f, 20);
	ImGui::End();
}
void drawImGuiWindow_modelInfo(Model m) {
	ImGui::Begin("Model info");
	//Prepare stream
	std::ostringstream oss;
	//Texture info
	ImGui::SeparatorText("Textures");
	//oss << "Textures: " << m.textures_loaded.size();
	//ImGui::Text(oss.str().c_str());
	//oss.str("");
	//oss.clear();
	for (int i = 0; i < m.textures_loaded.size(); i++) {
		oss << "\t" << m.textures_loaded[0].id;
		oss << " \"" << m.textures_loaded[0].path;
		oss << "\" (" << m.textures_loaded[0].type << ")";
		ImGui::Text(oss.str().c_str());
		oss.str("");
		oss.clear();
	}
	ImGui::SeparatorText("Mesh info");
	oss << "Meshes: " << m.getMeshCount();
	ImGui::Text(oss.str().c_str());
	oss.str("");
	oss.clear();
	//Vertices
	oss << "Vertices: " << UtilsNumbers::formatThousands((int)m.getVertexCount());
	ImGui::Text(oss.str().c_str());
	oss.str("");
	oss.clear();
	ImGui::End();
}