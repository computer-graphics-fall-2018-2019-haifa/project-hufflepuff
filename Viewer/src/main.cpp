#define _USE_MATH_DEFINES

#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <time.h>
#include <nfd.h>
#include <stdlib.h>
#include <memory>
#include <string>
#include <sstream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <filesystem>
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "ImguiMenus.h"
#include "Light.h"
#include "Utils.h"


int windowWidth = 1280, windowHeight = 720;
Renderer* r;
Scene* s;

// Function declarations
static void GlfwErrorCallback(int error, const char* description);
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	
	// Handle mouse scrolling here...
}

int main(int argc, char **argv)
{
	// initialize rand
	srand(static_cast <unsigned> (time(0)));

	// Create GLFW window
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
	{
		return 1;
	}

	// Move OpenGL context to the newly created window
	glfwMakeContextCurrent(window);

	// Create the renderer and the scene
	Renderer renderer;
	Scene scene;

	r = &renderer;
	s = &scene;

	renderer.LoadShaders();
	renderer.LoadTextures();

	Camera *c = new Camera(glm::vec3(0,0,5), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	scene.AddCamera(c);
	Light *l = new Light();
	scene.AddLight(l);

	// Setup ImGui
	ImGuiIO& io = SetupDearImgui(window);

	// Register a mouse scroll-wheel callback
	glfwSetScrollCallback(window, ScrollCallback);

	// Setup window events callbacks
	glfwSetFramebufferSizeCallback(window, glfw_OnFramebufferSize);

	//openGL things
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// This is the main game loop..
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		StartFrame();

		// Here we build the menus for the next frame. Feel free to pass more arguments to this function call
		DrawImguiMenus(io, scene, renderer);

		// Render the next frame
		RenderFrame(window, scene, renderer, io);
    }

	// If we're here, then we're done. Cleanup memory.
	Cleanup(window);
    return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
	s->GetActiveCamera().aspectRatio = width / height;
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);

	if (!glfwInit())
		return NULL;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();

	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	// Render the menus
	ImGui::Render();

	glfwMakeContextCurrent(window);

	glm::vec4 clearColor = GetClearColor();
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glEnable(GL_DEPTH_TEST);

	// Clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the scene
	renderer.Render(scene);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}
