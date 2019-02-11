#pragma once
#include "Scene.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include <vector>
#include <memory>
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
	int viewportWidth;
	int viewportHeight;
	int viewportX;
	int viewportY;
	int aliasViewportWidth;
	int aliasViewportHeight;
	float zNear;
	float zFar;

	ShaderProgram lightShader;
	ShaderProgram colorShader;

	/*void putPixel(int x, int y, double z, const glm::vec3& color, bool test = true);
	void createBuffers(int viewportWidth, int viewportHeight, glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f));
	int GetActiveViewportWidth();
	int GetActiveViewportHeight();*/

	GLuint glScreenTex;
	GLuint glScreenVtc;

	/*void createOpenGLBuffer();
	void initOpenGLRendering();*/

	glm::vec3 centerAxes;

public:
	bool alias;
	bool fogActivated;
	glm::vec3 fogColor;

	Renderer();
	~Renderer();

	void DrawModel(const Scene & scene, MeshModel* model);

	void Render(const Scene& scene);

	void LoadShaders();
	/*glm::vec3 centerPoint(glm::vec3 point);
	glm::vec3 centerPoint(glm::vec4 point);
	void DrawLine(const vec3& point1, const vec3& point2, const vec3& color);
	void DrawSquare(vec3 vertices[4], vec3 color);
	void DrawBoundingBox(glm::mat4 matrix, glm::vec3 min, glm::vec3 max);
	glm::vec3 CalcIllumination(Light& light, glm::vec3 p, glm::vec3 normal, glm::vec3 eye);
	glm::vec3 applyFog(glm::vec3 & color, float currentZ);
	void DrawTriangle(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, MeshModel* model, const Scene & scene, bool isLight);
	void DrawFaceNormal(std::vector<glm::vec3>& vertices, glm::mat4 m);
	void DrawAxes(const Scene & scene);
	void DrawModel(const Scene& scene, MeshModel* model, glm::mat4 matrix, bool lightModel = false);
	void InitZBuffer();*/
};
