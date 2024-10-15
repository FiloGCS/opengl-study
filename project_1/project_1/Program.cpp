#include <iostream>
#include <vector>
#include <stdlib.h>
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
#include "Light.h"
#include "Camera.h"

//GLOBAL VARIABLES-------------------------
//Window
ImVec4 clear_color = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
int window_width = 1920;
int window_height = 1080;
//Engine
float deltaTime = 0.0f;
float lastFrame = 0.0f;
std::vector<Entity> entities;
std::vector<Light> lights;
bool use_vSync = true;
//Camera
Camera camera;
float cameraSpeed = 1;
//Lighting
glm::vec3 ambient_color = glm::vec3(0.1, 0.1, 0.15);
glm::vec3 point1_color = glm::vec3(1, 0.95, 0.8);
glm::vec3 point1_position = glm::vec3(1, 1, 3);
float point1_falloff = 5;
//Interface
float nextStatsUpdateTime = 0;
float statsUpdateFreq = 4; //Times to update stats (per second)
float renderTime = 0;
//Debug
struct frameInfo {
	int FrameNumber;
	double time;
	float frameTime;
	float updateTime;
	float renderTime;
};
unsigned int frame = 0;
std::vector<frameInfo> frameHistory;
unsigned int frameHistoryMaxSize = 180;

std::vector<Shader*> loadedShaders;
static int selectedShader = 0;
int selectedEntity = 0;
bool onlyDrawSelectedEntity = false;
//Input - Mouse
float lastX = 400, lastY = 300;
bool firstMouse = true;

//FUNCTION PROTOTYPES-----------------------
//callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//ImGui window functions
void drawImGuiWindow_settings(GLFWwindow* window, bool& show_demo_window);
void drawImGuiWindow_stats(GLFWwindow* window);
void drawImGuiWindow_environment(GLFWwindow* window, glm::vec3& ambient, glm::vec3& point1, float& point1_falloff);
void drawImGuiWindow_modelInfo(Entity* e);

//MAIN ----------------------------------------------------------------------------
int main() {

	//GLFW------------------------------
	cout << "Initializing GLFW...\t";
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
	cout << " Done!" << endl;

	//ImGUI-----------------------------
	cout << "Initializing ImGui...\t";
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
	cout << " Done!" << endl;

	//GLAD------------------------------
	cout << "Initializing GLAD...\t";
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	cout << " Done!" << endl;

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

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	//Setup Camera
	camera = Camera(glm::vec3(0.0f, 0.0f, -4.0f));

	//ADD ENTITIES-------------
	double t0 = glfwGetTime();
	cout << "Loading entities..." << endl;
	//Default entities
	float default_size = 0.25f;
	int default_entities_n = 1;
	entities.reserve(default_entities_n);
	for (int i = 0; i < default_entities_n; ++i) {
		entities.emplace_back();
	}
	//Set entities size
	for (int i = 0; i < entities.size(); i++) {
		entities[i].scale *= default_size;
		glm::vec3 randomOffset = glm::vec3(rand()%100*0.01f-0.5f, rand()%100*0.01f-0.5f, rand()%100*0.01f-0.5f);
		entities[i].position += randomOffset;
	}
	//Specific entities

	double t1 = glfwGetTime();
	cout << "Entities loaded in " << (t1 - t0) * 1000 << " miliseconds!" << endl;

	//Launch Start for all entities
	for (int i = 0; i < entities.size(); i++) {
		entities[i].Start();
	}

	//ADD LIGHTS---------------
	Light point1 = Light(point1_position, "point1");
	lights.push_back(point1);

	//COMPILING SHADERS...
	t0 = glfwGetTime();
	cout << "Compiling shaders...\t";
	Shader shader1 = Shader("default", "Entity Shader");
	loadedShaders.push_back(&shader1);
	Shader shader4 = Shader("default_Flat", "Flat Shading");
	loadedShaders.push_back(&shader4);
	Shader shader2 = Shader("default_UV", "UV");
	loadedShaders.push_back(&shader2);
	Shader shader3 = Shader("default_Normal", "World Normal");
	loadedShaders.push_back(&shader3);
	t1 = glfwGetTime();
	cout << " done in " << (t1 - t0) * 1000 << " miliseconds!" << endl;


	/// START RENDER LOOP
	while (!glfwWindowShouldClose(window)) {

		//HACK ENABLE VSYNC in the loop for debug purposes
		if (use_vSync) {
			glfwSwapInterval(3);
		}
		else {
			glfwSwapInterval(0);
		}

		//Update frame information
		frameInfo currentFrameInfo;
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		currentFrameInfo.time = glfwGetTime();
		currentFrameInfo.FrameNumber = frame++;
		frame++;

		//UPDATE INPUT
		glfwPollEvents();

		glm::vec3 camera_move_direction = glm::vec3(0);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cout << "W" << endl;
			camera_move_direction += glm::vec3(0, 0, 1);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cout << "A" << endl;
			camera_move_direction += glm::vec3(1, 0, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cout << "S" << endl;
			camera_move_direction += glm::vec3(0, 0, -1);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cout << "D" << endl;
			camera_move_direction += glm::vec3(-1, 0, 0);
		}
		//camera_move_direction = glm::normalize(camera_move_direction);
		camera.position += camera_move_direction * deltaTime;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Update transform matrices
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / window_height, 0.1f, 100.0f);

		//UPDATE ENTITIES
		double tUpdate0 = glfwGetTime();
		for (Entity& entity : entities) {
			entity.Update();
		}
		currentFrameInfo.updateTime = (glfwGetTime() - tUpdate0)*1000;

		//CLEAR BUFFERS
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//RENDER OPAQUE

		double tRender0 = glfwGetTime();
		GLuint queryID;//Preparing OpenGL Query to measure render time
		glGenQueries(1, &queryID);
		glBeginQuery(GL_TIME_ELAPSED, queryID);
		bool statsdrawn = false;
		for (int i = 0; i < entities.size(); i++) {
			Entity* entity = &entities.at(i);
			if (selectedEntity == i) {
				drawImGuiWindow_modelInfo(&(entities.at(selectedEntity)));
			}
			if (!onlyDrawSelectedEntity || selectedEntity == i) {
				//Load the material's shader, or the selected debug material
				Shader* shader = &(entity->shader);
				if (selectedShader != 0) {
					shader = loadedShaders.at(selectedShader);
				}
				shader->use();
				//Setting engine uniforms
				shader->setVector2("resolution", window_width, window_height);
				shader->setFloat("time", glfwGetTime());
				//Setting lighting uniforms
				shader->setVector3("ambient_color", ambient_color);
				shader->setVector3("point1_position", point1_position);
				shader->setVector3("point1_color", point1_color);
				shader->setFloat("point1_falloff", point1_falloff);
				//Render
				entity->Render(projection, view, shader);
				//Show Debug info
			}
		}
		currentFrameInfo.renderTime = (glfwGetTime() - tRender0)*1000;

		//TODO - RENDER TRANSLUCENT
		// [...]


		//DEBUG
		//Save this frame's info
		currentFrameInfo.frameTime = (glfwGetTime() - currentFrameInfo.time);
		frameHistory.push_back(currentFrameInfo);
		if (frameHistory.size() > frameHistoryMaxSize) {
			frameHistory.erase(frameHistory.begin());
		}

		//RENDER IMGUI
		/*if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}*/
		drawImGuiWindow_stats(window);
		drawImGuiWindow_environment(window, ambient_color, point1_color, point1_falloff);


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
void drawImGuiWindow_stats(GLFWwindow* window) {
	ImGui::Begin("Stats");

	std::ostringstream oss;
	oss.precision(3);
	oss << "Frame " << frame; 
	ImGui::Text(oss.str().c_str());
	if (frameHistory.size() > 0) {
		oss.str("");
		oss.clear();
		float tDelta = frameHistory.back().time - frameHistory.begin()->time;
		float n = frameHistory.size();
		float fps = n / tDelta;
		oss.precision(1);
		oss << " - Average FPS: " << std::fixed << fps;
		ImGui::SameLine();
		ImGui::Text(oss.str().c_str());
		oss.precision(3);
	}
	oss.str("");
	oss.clear();
	oss << std::fixed << frameHistory.back().updateTime << "ms Update";
	ImGui::PlotHistogram(oss.str().c_str(), &frameHistory.at(0).updateTime, frameHistory.size(), 0, NULL, 0.0f, 5.0f, ImVec2(0, 30.0f), (int)sizeof(frameInfo));

	oss.str("");
	oss.clear();
	oss << std::fixed << frameHistory.back().renderTime << "ms Render";
	ImGui::PlotHistogram(oss.str().c_str(), &frameHistory.at(0).renderTime, frameHistory.size(), 0, NULL, 0.0f, 5.0f, ImVec2(0, 30.0f), (int)sizeof(frameInfo));
	ImGui::End();
}
void drawImGuiWindow_environment(GLFWwindow* window, glm::vec3& ambient, glm::vec3& point1, float& point1_falloff) {

	ImGui::Begin("Environment");
	//const char* items[] = { "default", "UV_CHECK"};
	//ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
	ImGui::SeparatorText("Camera");
	ImGui::InputFloat3("Position", &(camera.position[0]));
	ImGui::Checkbox("vSync", &use_vSync);
	ImGui::SeparatorText("Shader Picker");
	//Current Shader info
	std::ostringstream oss;
	GLint binaryLength;
	glGetProgramiv(loadedShaders.at(selectedShader)->ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
	oss << "(" <<binaryLength << " bytes) - " << loadedShaders.at(selectedShader)->name;
	ImGui::Text(oss.str().c_str());

	int buttons_per_line = 3;
	for (int i = 0; i < loadedShaders.size(); i++) {
		ImGui::RadioButton(loadedShaders.at(i)->name.c_str(), &selectedShader, i);
		bool same_line = (i + 1) % buttons_per_line != 0
			&& (i + 1) < loadedShaders.size();
		if (same_line) {
			ImGui::SameLine();
		}
	}
	//ImGui::InputInt(loadedShaders.at(selectedShader%loadedShaders.size())->name.c_str(), &selectedShader);
	//selectedShader = selectedShader % loadedShaders.size();
	ImGui::SeparatorText("Environment");
	//TODO - Reset button
	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	ImGui::ColorEdit3("ambient light", (float*)&ambient);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	ImGui::SeparatorText("Point Lights");
	if (ImGui::BeginTabBar("Point Lights Tabs", tab_bar_flags)) {
		if (ImGui::BeginTabItem("Point Light 1")) {
			ImGui::InputFloat3("position", &point1_position.x);
			ImGui::ColorEdit3("color", (float*)&point1);
			ImGui::SliderFloat("falloff", &point1_falloff, 0.25f, 20);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}
void drawImGuiWindow_modelInfo(Entity* e) {
	Model m = e->model;
	ImGui::Begin("Entity inspector");

	ImGui::SeparatorText("Selected:");
	ImGui::InputInt("TO DO", &selectedEntity);
	selectedEntity = selectedEntity % entities.size();
	ImGui::Checkbox("Isolate selected", &onlyDrawSelectedEntity);
	//Prepare stream
	std::ostringstream oss;
	//Transform
	ImGui::SeparatorText("Transform");
	ImGui::InputFloat3("Position", &(e->position[0]));
	ImGui::InputFloat4("Rotation", &(e->rotation[0]));
	ImGui::InputFloat3("Scale", &(e->scale[0]));
	//Texture info
	ImGui::SeparatorText("Textures");
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