#pragma once
#include "Scene.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

typedef glm::vec3 vec3;

/*
 * Renderer class.
 */
class Renderer
{
private:
	float *colorBuffer;
	float *zBuffer;
	int viewportWidth;
	int viewportHeight;
	int viewportX;
	int viewportY;

	void putPixel(int x, int y, const glm::vec3& color);
	void createBuffers(int viewportWidth, int viewportHeight);

	GLuint glScreenTex;
	GLuint glScreenVtc;

	void createOpenGLBuffer();
	void initOpenGLRendering();

	glm::vec3 centerAxes;

public:
	Renderer(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	~Renderer();

	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void SetViewport(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);

	// Add more methods/functionality as needed...
	glm::vec3 centerPoint(glm::vec3 point);
	glm::vec3 centerPoint(glm::vec4 point);
	void DrawLine(const vec3& point1, const vec3& point2, const vec3& color);
	void DrawSquare(vec3 vertices[4], vec3 color);
	void DrawBoundingBox(glm::mat4 matrix, glm::vec3 min, glm::vec3 max);
	void DrawTriangle(std::vector<glm::vec3>& vertices, glm::vec3 & color);
	void DrawFaceNormal(std::vector<glm::vec3>& vertices, glm::mat4 m);
	void DrawAxes(const Scene & scene);
	void DrawModel(MeshModel* model, glm::mat4 matrix);
};
