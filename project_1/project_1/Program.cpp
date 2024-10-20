﻿#include <iostream>
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
#include "Core/Shader.h"
#include "Core/Texture2D.h" 
#include "Core/Entity.h"
#include "Core/Model.h"
#include "Core/Light.h"
#include "Core/Camera.h"
#include "Utils/UtilsNumbers.h"
//stb_image library handles image loading
//This needs to be last since we're #including stb_image.h in previous includes
// and we can only implement it once!
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
bool use_opaque_pass = true;
bool use_translucent_pass = true;
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};
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
std::vector<Shader*> loadedPostprocessShaders;
static int selectedPostprocessShader = 0;
int selectedEntity = 0;
bool onlyDrawSelectedEntity = false;
//Input - Mouse
float lastX = 400, lastY = 300;
bool firstMouse = true;

//FUNCTION PROTOTYPES-----------------------
void processInput(GLFWwindow* window, Camera* camera);
//Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//Cubemap
unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadCubemap(string skyboxName);
//Render functions
void renderEntities(vector<Entity*> entities, Shader* ghostShader, glm::mat4 projection, glm::mat4 view);
//ImGui window functions
void drawImGuiWindow_settings(GLFWwindow* window, bool& show_demo_window);
void drawImGuiWindow_stats(GLFWwindow* window);
void drawImGuiWindow_environment(GLFWwindow* window, glm::vec3& ambient, glm::vec3& point1, float& point1_falloff);
void drawImGuiWindow_modelInfo(Entity* e);
void drawImGuiWindow_cameraInfo(Camera* cam);

//MAIN ----------------------------------------------------------------------------
int main() {

	//Seed random with time
	srand(time(NULL));

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
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Real Engine", NULL, NULL);
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
	//glfwSetKeyCallback(window, key_callback);
	cout << " Done!" << endl;

	//HERE WE GO!-----------------------
	glViewport(0, 0, window_width, window_height);
	//STENCIL BUFFER
	glEnable(GL_STENCIL_TEST);
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

	//FACE CULLING
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	//Setup Camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 4.0f));

	//Setup screen quad
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//COMPILING SHADERS...
	double t0 = glfwGetTime();
	cout << "Compiling shaders...\t";
	//Basic shaders
	Shader shaderBasicOpaque = Shader("basic_opaque", "Entity Shader");
	Shader shaderBasicTranslucent = Shader("basic_translucent", "Basic Translucent");
	shaderBasicTranslucent.blendMode = Translucent;
	//Debug Shaders
	//TODO - should redo the system, having the default in loadedShaders[0] doesn't make much sense...
	Shader shader2 = Shader("debug/debug_uv", "TexCoord");
	Shader shader3 = Shader("debug/debug_normal", "World Normal");
	Shader shader4 = Shader("debug/debug_flat", "Flat");
	loadedShaders.push_back(&shaderBasicOpaque);
	loadedShaders.push_back(&shader2);
	loadedShaders.push_back(&shader3);
	loadedShaders.push_back(&shader4);
	//Engine shaders
	string quadShaderPath = "Assets/Shaders/engine/quad.vert";
	Shader ghostedShader = Shader("engine/ghosted", "Ghosted");
	//Postprocess shaders
	string postprocessPath = "Assets/Shaders/postprocess/";
	//TODO - Could do with a for loop?
	Shader postprocessShader1 = Shader(quadShaderPath, postprocessPath+"none.frag", "None");
	Shader postprocessShader2 = Shader(quadShaderPath, postprocessPath+"blur.frag", "Blur");
	Shader postprocessShader3 = Shader(quadShaderPath, postprocessPath+"sharpen.frag", "Sharpen");
	Shader postprocessShader4 = Shader(quadShaderPath, postprocessPath+"edges.frag", "Edges");
	Shader postprocessShader5 = Shader(quadShaderPath, postprocessPath+"grayscale.frag", "Grayscale");
	Shader postprocessShader6 = Shader(quadShaderPath, postprocessPath+"inversion.frag", "Invert");
	loadedPostprocessShaders.push_back(&postprocessShader1);
	loadedPostprocessShaders.push_back(&postprocessShader2);
	loadedPostprocessShaders.push_back(&postprocessShader3);
	loadedPostprocessShaders.push_back(&postprocessShader4);
	loadedPostprocessShaders.push_back(&postprocessShader5);
	loadedPostprocessShaders.push_back(&postprocessShader6);
	//Skybox shaders
	string skyboxPath = "Assets/Shaders/skybox/";
	Shader gradientShader = Shader(quadShaderPath, skyboxPath+"gradient_01.frag", "Gradient Background");
	Shader skyboxShader = Shader("skybox/skybox", "Skybox");

	double t1 = glfwGetTime();
	cout << " done in " << (t1 - t0) * 1000 << " miliseconds!" << endl;

	//LOAD SKYBOX CUBEMAP
	//Skybox VAO
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//Skybox faces paths
	string skyboxName = "skybox_01";
	unsigned int skyboxTexture = loadCubemap(skyboxName);

	//LOADING MODELS
	char DEFAULT_MODEL_PATH[] = "Assets/Models/cube_smooth/cube_corners.obj";
	Model defaultModel = Model(DEFAULT_MODEL_PATH);

	//CREATE ENTITIES
	t0 = glfwGetTime();
	cout << "Loading entities..." << endl;
	//Creating default shader
	//Default entities
	float default_size = 0.2f;
	int default_entities_n = 20;
	entities.reserve(default_entities_n);
	//For each desired entity
	for (int i = 0; i < default_entities_n; ++i) {
		//Create a new Entity in the entities vector
		entities.emplace_back();
		//Use the default model
		entities.back().model = &defaultModel;
		//Scale them by the default scale
		entities.back().scale *= default_size;
		//Randomize the position
		glm::vec3 randomOffset = UtilsNumbers::getRandomOffset();
		entities.back().position += randomOffset;
		//Choose between opaque or translucent shader at random
		entities.back().shader = (rand() % 2 == 0) ? &shaderBasicOpaque : &shaderBasicTranslucent;
	}

	t1 = glfwGetTime();
	cout << "Entities loaded in " << (t1 - t0) * 1000 << " miliseconds!" << endl;

	//Launch Start for all entities
	for (int i = 0; i < entities.size(); i++) {
		entities[i].Start();
	}

	//ADD LIGHTS---------------
	Light point1 = Light(point1_position, "point1");
	lights.push_back(point1);


	//FRAMEBUFFER SETUP
	//Creating framebuffer object
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	//generate texture attachment
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach it to the currently bound framebuffer object as a color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	//For depth-stencil attachment we can use a renderbuffer instead of texture since we don't need sampling
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
	//After allocating memory for our new renderbuffer, we can unbind it
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//Finally we attach this depth-stencil renderbuffer attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//Check that the framebuffer is complete:
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	//Unbind the framebuffer, we don't want to accidentally render to it
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	///RENDER LOOP ================================================
	while (!glfwWindowShouldClose(window)) {

		//HACK ENABLE VSYNC in the loop for debug purposes
		if (use_vSync) {
			glfwSwapInterval(1);
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
		processInput(window, &camera);

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
		currentFrameInfo.updateTime = (glfwGetTime() - tUpdate0) * 1000;

		//START MEASUREMENT ----------------------
		double tRender0 = glfwGetTime();
		GLuint queryID;//Preparing OpenGL Query to measure render time
		glGenQueries(1, &queryID);
		glBeginQuery(GL_TIME_ELAPSED, queryID);

		//RENDER PREPARATIONS
		//Sort Entities by camera distance
		glm::vec3 cameraPos = camera.position;
		std::sort(entities.begin(), entities.end(),
			[&cameraPos](const Entity& a, const Entity& b) {
				return Entity::compareDistanceToPoint(a, b, cameraPos);
			}
		);
		//Sort into two different vectors, opaque and translucent
		vector<Entity*> opaqueEntities = vector<Entity*>();
		vector<Entity*> translucentEntities = vector<Entity*>();
		for (int i = 0; i < entities.size(); i++) {
			//If any debug setting is overrading the material...
			//Ghosting/Isolation
			if (entities[i].isVisible) {
				if (onlyDrawSelectedEntity && entities[i].ID != selectedEntity) {
					opaqueEntities.push_back(&entities[i]);
				}
				else {
					//Add to the respective list
					Shader* shaderBeingUsed = (selectedShader != 0)?
						loadedShaders[selectedShader]:
						entities[i].shader;
					switch (shaderBeingUsed->blendMode) {
					case Opaque:
						opaqueEntities.push_back(&entities[i]);
						break;
					case Translucent:
						translucentEntities.push_back(&entities[i]);
						break;
					default:
						break;
					}
				}
			}
		}

		//BIND OUR CUSTOM FRAMEBUFFER
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//Clear buffer
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//RENDER BACKGROUND GRADIENT
		bool render_gradient = false;
		if (render_gradient) {
			//Use our quad shader
			gradientShader.use();
			gradientShader.setVector2("resolution", window_width, window_height);
			gradientShader.setFloat("time", glfwGetTime());
			//Use our wuad VAO
			glBindVertexArray(quadVAO);
			glDisable(GL_DEPTH_TEST); //We don't want the screen quad to depth test
			glDepthMask(GL_FALSE);
			//glBindTexture(GL_TEXTURE_2D, textureColorbuffer); //We can use texture attachment in our shader :)
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
		}
		//RENDER SKYBOX
		bool render_skybox = true;
		if (render_skybox) {
			glDepthMask(GL_FALSE);
			skyboxShader.use();
			//... set view and projection matrix
			skyboxShader.setMat4("projection", projection);
			skyboxShader.setMat4("view", view);
			glBindVertexArray(skyboxVAO); //TODO
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthMask(GL_TRUE);
		}
		//RENDER OPAQUE
		if (use_opaque_pass) {
			glDisable(GL_BLEND);
			renderEntities(opaqueEntities, &ghostedShader, projection, view);
		}

		//RENDER TRANSLUCENT
		if (use_translucent_pass) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
			renderEntities(translucentEntities, &ghostedShader, projection, view);
			glDepthMask(GL_TRUE);
		}

		//END RENDER ---------------------------
		currentFrameInfo.renderTime = (glfwGetTime() - tRender0) * 1000;

		//RENDER POSTPROCESS
		//Bind default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		//Use our quad shader
		loadedPostprocessShaders[selectedPostprocessShader]->use();
		//Use our wuad VAO
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST); //We don't want the screen quad to depth test
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer); //We can use texture attachment in our shader :)
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//DEBUG
		//Save this frame's info
		currentFrameInfo.frameTime = (glfwGetTime() - currentFrameInfo.time);
		frameHistory.push_back(currentFrameInfo);
		if (frameHistory.size() > frameHistoryMaxSize) {
			frameHistory.erase(frameHistory.begin());
		}

		//RENDER IMGUI
		//if (show_demo_window) {
		//	ImGui::ShowDemoWindow(&show_demo_window);
		//}
		drawImGuiWindow_modelInfo(&(entities.at(selectedEntity)));
		drawImGuiWindow_cameraInfo(&camera);
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
//Input functions
//TODO use callbacks?
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F && action == GLFW_PRESS); {
		camera.isFocused = !camera.isFocused;
	}
}
void processInput(GLFWwindow* window, Camera* cam) {
	glm::vec3 camera_move_direction = glm::vec3(0);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//camera_move_direction += glm::vec3(0, 0, 1);
		camera_move_direction += glm::normalize(camera.rotation * glm::vec3(0, 0, -1));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//camera_move_direction += glm::vec3(-1, 0, 0);
		camera_move_direction += glm::normalize(camera.rotation * glm::vec3(-1, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//camera_move_direction += glm::vec3(0, 0, -1);
		camera_move_direction += glm::normalize(camera.rotation * glm::vec3(0, 0, 1));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//camera_move_direction += glm::vec3(1, 0, 0);
		camera_move_direction += glm::normalize(camera.rotation * glm::vec3(1, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		glm::quat rot = glm::angleAxis(glm::radians(-90 * deltaTime), glm::vec3(0, 01, 0));
		cam->rotation = rot * cam->rotation;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glm::quat rot = glm::angleAxis(glm::radians(90 * deltaTime), glm::vec3(0, 01, 0));
		cam->rotation = rot * cam->rotation;
	}
	if (cam->isFocused) {
		float length = glm::length(cam->position);
		cam->position = camera.rotation * glm::vec3(0, 0, length);
	}
	//camera_move_direction = glm::normalize(camera_move_direction);
	cam->position += camera_move_direction * deltaTime;
}
//Cubemaps
unsigned int loadCubemap(vector<std::string> faces) {
	//Apparently, cubemaps have been specified to follow the RenderMan specification (for some reason).
	// That means that, contrary to the usual OpenGL convention of having the image origin
	// be the lower left, for cubemaps the origin is the top left.
	// https://stackoverflow.com/questions/11685608/convention-of-faces-in-opengl-cubemapping
	// So wee need to disable flip_vertically_on_load during the cubemap loading process.
	stbi_set_flip_vertically_on_load(false);
	//Creating the cubemap texture
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	//Loading data into the cubemap faces
	int width, height, nrChannels;
	//For each of the faces in the vector
	for (unsigned int i = 0; i < faces.size(); ++i) {
		//Load the i-th path and use it to load image data using stbi_load
		unsigned char* data;
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			//Since the faces are contiguous we can just add i to the first one
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);
	return textureID;
}
unsigned int loadCubemap(string name) {
	vector<string> faces{
		"Assets/Textures/Cubemaps/" + name + "/right.jpg",
		"Assets/Textures/Cubemaps/" + name + "/left.jpg",
		"Assets/Textures/Cubemaps/" + name + "/top.jpg",
		"Assets/Textures/Cubemaps/" + name + "/bottom.jpg",
		"Assets/Textures/Cubemaps/" + name + "/front.jpg",
		"Assets/Textures/Cubemaps/" + name + "/back.jpg"
	};
	return loadCubemap(faces);
}
//Render functions
void renderEntities(vector<Entity*> entities, Shader* ghostShader, glm::mat4 projection, glm::mat4 view) {

	//Render a vector of entities
	Shader* sh;
	for (int i = 0; i < entities.size(); ++i) {
		//TODO - Should use entity ID instead of just i, since reordering the list messes this up
		if (onlyDrawSelectedEntity && selectedEntity != entities[i]->ID) {
			//Use ghost shader
			sh = ghostShader;
		}
		else {
			//Use the selected shader or the entity's shader if none is selected
			sh = (selectedShader != 0) ? loadedShaders.at(selectedShader) : entities[i]->shader;
		}
		//Use Shader
		sh->use();
		//Setting engine uniforms
		sh->setVector2("resolution", window_width, window_height);
		sh->setFloat("time", glfwGetTime());
		//Setting lighting uniforms
		sh->setVector3("ambient_color", ambient_color);
		sh->setVector3("point1_position", point1_position);
		sh->setVector3("point1_color", point1_color);
		sh->setFloat("point1_falloff", point1_falloff);
		//Render
		entities[i]->Render(projection, view, sh);
	}
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
	ImGui::SeparatorText("Render passes");
	ImGui::Checkbox("Opaque pass", &use_opaque_pass);
	ImGui::Checkbox("Translucent pass", &use_translucent_pass);
	
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
	//Postprocessing 
	ImGui::SeparatorText("Postprocessing");
	std::ostringstream oss;
	//Current Shader info
	GLint binaryLength;
	glGetProgramiv(loadedPostprocessShaders.at(selectedPostprocessShader)->ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
	oss << "(" << binaryLength << " bytes) - " << loadedPostprocessShaders.at(selectedPostprocessShader)->name;
	ImGui::Text(oss.str().c_str());

	int buttons_per_line = 3;
	for (int i = 0; i < loadedPostprocessShaders.size(); i++) {
		ImGui::RadioButton(loadedPostprocessShaders.at(i)->name.c_str(), &selectedPostprocessShader, i);
		bool same_line = (i + 1) % buttons_per_line != 0
			&& (i + 1) < loadedPostprocessShaders.size();
		if (same_line) {
			ImGui::SameLine();
		}
	}
	ImGui::End();
}
void drawImGuiWindow_modelInfo(Entity* e) {
	Model m = *(e->model);
	ImGui::Begin("Entity inspector");
	std::ostringstream oss;
	oss << "Selected: " << entities[selectedEntity].name;
	ImGui::SeparatorText(oss.str().c_str());
	oss.str("");
	oss.clear();
	ImGui::InputInt("TO DO", &selectedEntity);
	selectedEntity = selectedEntity % entities.size();
	ImGui::Checkbox("Isolate selected", &onlyDrawSelectedEntity);
	//Prepare stream
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
	ImGui::SeparatorText("Shader Picker");
	//Current Shader info
	GLint binaryLength;
	glGetProgramiv(loadedShaders.at(selectedShader)->ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
	oss << "(" << binaryLength << " bytes) - " << loadedShaders.at(selectedShader)->name;
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
	ImGui::End();
}
void drawImGuiWindow_cameraInfo(Camera* cam) {

	ImGui::Begin("Camera info");
	ImGui::Checkbox("Locked", &camera.isFocused);
	ImGui::SeparatorText("Transform");
	ImGui::InputFloat3("Position", &(cam->position[0]));
	ImGui::InputFloat4("Rotation", &(cam->rotation[0]));
	ImGui::End();
}