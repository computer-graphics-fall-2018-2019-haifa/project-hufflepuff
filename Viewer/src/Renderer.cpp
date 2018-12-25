#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include "Utils.h"
#include <iostream>
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include <math.h>
#include <algorithm>

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define ZINDEX(w,x,y) ((x)+(y*w))
#define FLAT 0
#define GOURAUD 1
#define PHONG 2

const double VERYBIG = std::numeric_limits<float>::max();
typedef glm::vec3 vec3;
int x_add;
int y_add;

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr),
	fogColor(0.8353f, 0.7804f, 0.9098f)
{
	initOpenGLRendering();
	alias = false;
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);

	this->centerAxes = glm::vec3((int)viewportWidth / 2, (int)viewportHeight / 2, 0);
}

Renderer::~Renderer()
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}
}

void Renderer::putPixel(int x, int y, double z, const glm::vec3& color, bool test)
{
	int w = GetActiveViewportWidth();
	int h = GetActiveViewportHeight();
	if (x < 0) return; if (x >= w) return;
	if (y < 0) return; if (y >= h) return;
	int idx = ZINDEX(w, x, y);
	if (!test || z < zBuffer[idx]) {
		zBuffer[idx] = z;
		colorBuffer[INDEX(w, x, y, 0)] = color.x;
		colorBuffer[INDEX(w, x, y, 1)] = color.y;
		colorBuffer[INDEX(w, x, y, 2)] = color.z;
	}
}

void Renderer::createBuffers(int viewportWidth, int viewportHeight, glm::vec3 color)
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}
	if (zBuffer) {
		delete[] zBuffer;
	}

	colorBuffer = new float[3* viewportWidth * viewportHeight];
	zBuffer = new float[viewportWidth * viewportHeight];
	for (int x = 0; x < viewportWidth; x++)
	{
		for (int y = 0; y < viewportHeight; y++)
		{
			putPixel(x, y, VERYBIG, color);
		}
	}
}

int Renderer::GetActiveViewportWidth()
{
	return alias ? aliasViewportWidth : viewportWidth;
}

int Renderer::GetActiveViewportHeight()
{
	return alias ? aliasViewportHeight : viewportHeight;
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	int w = GetActiveViewportWidth();
	int h = GetActiveViewportHeight();
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			putPixel(i, j, VERYBIG-1, color, false);
		}
	}
}

void Renderer::SetViewport(int viewportWidth, int viewportHeight, int viewportX, int viewportY)
{
	this->viewportX = viewportX;
	this->viewportY = viewportY;
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	int x_add = (int)(viewportWidth / 2);
	int y_add = (int)(viewportHeight / 2);

	if (alias) {
		this->aliasViewportWidth = this->viewportWidth * 2;
		this->aliasViewportHeight = this->viewportHeight * 2;
		createBuffers(this->aliasViewportWidth, this->aliasViewportHeight);
		x_add *= 2; y_add *= 2;
	}
	else {
		createBuffers(this->viewportWidth, this->viewportHeight);
	}
	createOpenGLBuffer();

	centerAxes = vec3(x_add, y_add, 0);
}

void Renderer::SetViewport()
{
	SetViewport(this->viewportWidth, this->viewportHeight);
}

glm::vec3 Renderer::centerPoint(glm::vec3 point)
{
	return point + centerAxes;
}

glm::vec3 Renderer::centerPoint(glm::vec4 point)
{
	return Utils::Vec3FromVec4(point) + centerAxes;
}

float CalcZOfPointOnLine(glm::vec3 start, glm::vec3 end, float x) {
	glm::vec3 normal = glm::normalize(start - end);
	float t;
	if (normal.x) {
		t = (x - start.x) / normal.x;
	}
	else {
		t = 0;
	}
	glm::vec3 p = start + normal * t;
	return p.z;
}

void Renderer::DrawLine(const vec3& point1, const vec3& point2, const vec3& color = glm::vec3(0,0,0)) {
	// Bresenham's line algorithm
	float
		x1 = point1.x,
		y1 = point1.y,
		x2 = point2.x,
		y2 = point2.y;

	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));

	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;
	const int maxX = (int)x2;
	float z;

	for (int x = (int)x1; x < maxX; x++)
	{
		if (steep) {
			z = CalcZOfPointOnLine(point1, point2, y);
			putPixel(y, x, z, color);
		}
		else {
			z = CalcZOfPointOnLine(point1, point2, x);
			putPixel(x, y, z, color);
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}

void Renderer::DrawSquare(vec3 vertices[4], vec3 color) {
	vec3 a = vertices[0],
		b = vertices[1],
		c = vertices[2],
		d = vertices[3];
	DrawLine(a, b, color);
	DrawLine(a, d, color);
	DrawLine(b, c, color);
	DrawLine(c, d, color);
}

void Renderer::DrawBoundingBox(glm::mat4 matrix, glm::vec3 min, glm::vec3 max)
{
	/*
	 * points are represented by 3 sets of options:
	 * 1.   front | back
	 * 2.   top   | bottom
	 * 3.   left  | right
	 * 
	 */
	vec3
		ftl = centerPoint(matrix * glm::vec4(min.x, max.y, min.z, 1)),
		ftr = centerPoint(matrix * glm::vec4(max.x, max.y, min.z, 1)),
		fbl = centerPoint(matrix * glm::vec4(min.x, min.y, min.z, 1)),
		fbr = centerPoint(matrix * glm::vec4(max.x, min.y, min.z, 1)),
		btl = centerPoint(matrix * glm::vec4(min.x, max.y, max.z, 1)),
		btr = centerPoint(matrix * glm::vec4(max.x, max.y, max.z, 1)),
		bbl = centerPoint(matrix * glm::vec4(min.x, min.y, max.z, 1)),
		bbr = centerPoint(matrix * glm::vec4(max.x, min.y, max.z, 1));
	vec3 color = vec3(0, 0.5, 0.5);

	vec3 upperSquare[4] = { ftl, ftr, btr, btl };
	vec3 lowerSquare[4] = { fbl, fbr, bbr, bbl };
	vec3 rightSquare[4] = { ftr, btr, bbr, fbr };
	vec3 leftSquare[4]  = { ftl, btl, bbl, fbl };

	DrawSquare(lowerSquare, color);
	DrawSquare(upperSquare, color);
	DrawSquare(rightSquare, color);
	DrawSquare(leftSquare, color);
}

glm::vec3 Renderer::CalcIllumination(Light& light, glm::vec3 p, glm::vec3 normal, glm::vec3 eye)
{
	glm::vec3 l = centerPoint(light.location),
		lightVector = light.isPoint ? glm::normalize(p - l) : glm::normalize(-l),
		mirror = 2.0f * (glm::dot(normal, lightVector)) - lightVector,
		lookDirection = glm::normalize(p - eye);
	float brightness = glm::max(glm::dot(normal, lightVector), 0.0f);
	float cosPhi = glm::dot(lookDirection, mirror);
	glm::vec3 illumination;

	light.ambient = light.Ka * light.color;
	light.diffuse = light.Kd * brightness * light.color;
	light.specular = light.Ks * pow(cosPhi, light.shineOn) * light.color;

	illumination = light.ambient + light.diffuse + light.specular;

	if (fogActivated)
		illumination = applyFog(illumination, p.z);

	return illumination;
}

glm::vec3 Renderer::applyFog(glm::vec3& color, float currentZ) {
	float fogFactor = (zFar - abs(currentZ)) / (zFar - zNear);
	fogFactor = glm::clamp(fogFactor, 0.0f, 1.0f);
	return glm::mix(fogColor, color, fogFactor);
}

void Renderer::DrawTriangle(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, MeshModel* model, const Scene& scene, bool isLight) {
	glm::vec3 p1 = vertices[0],
		p2 = vertices[1],
		p3 = vertices[2];
	int minX = glm::min(p1.x, glm::min(p2.x, p3.x)),
		minY = glm::min(p1.y, glm::min(p2.y, p3.y)),
		maxX = glm::max(p1.x, glm::max(p2.x, p3.x)),
		maxY = glm::max(p1.y, glm::max(p2.y, p3.y));
	std::vector<Light*> lights = scene.GetLights();
	glm::vec3 eye = scene.GetActiveCamera().location;
	//eye = Utils::Mult(Utils::GetRotationMatrix(scene.GetActiveCamera().rotation), eye);
	glm::vec3 faceNormal = Utils::CalcFaceNormal(vertices);
	glm::vec3 c1 = model->color,
		c2 = model->color2;
	glm::vec3 p1Color = vec3(0), p2Color = vec3(0), p3Color = vec3(0);

	for (int i = 0; i < lights.size(); i++) {
		Light* light = lights[i];
		p1Color += CalcIllumination(*light, p1, normals[0], eye) * c1;
		p2Color += CalcIllumination(*light, p2, normals[1], eye) * c1;
		p3Color += CalcIllumination(*light, p3, normals[2], eye) * c1;
	}

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			glm::vec3 pxy(x, y, 0);
			glm::vec2 bar = Utils::GetBarycentricCoords(pxy, vertices);
			if (Utils::InTriangle(bar)) {
				float lambda1 = 1 - bar.x - bar.y;
				float lambda2 = bar.x;
				float lambda3 = bar.y;
				float z = 1 / (lambda1 * (1 / p1.z) + lambda2 * (1 / p2.z) + lambda3 * (1 / p3.z));
				pxy.z = z;
				glm::vec3 gouraudColor = (lambda1 * p1Color + lambda2 * p2Color + lambda3 * p3Color);
				glm::vec3 normalush = -glm::normalize(lambda1 * normals[0] + lambda2 * normals[1] + lambda3 * normals[2]);
				glm::vec3 finalColor = (scene.shadingType == GOURAUD) ? gouraudColor : c1;
				glm::vec3 finalNormal = (scene.shadingType == FLAT) ? faceNormal : normalush;

				glm::vec3 illumination(0);
				for (int i = 0; i < lights.size(); i++) {
					Light* light = lights[i];
					illumination += CalcIllumination(*light, pxy, finalNormal, eye);
				}

				if (!model->uniformMaterial)
					finalColor = Utils::GetMarbleColor(lambda1 + lambda3, c1, c2);

				if (isLight)
					illumination = glm::vec3(1);

				float r = glm::min(finalColor.r * illumination.r, 1.0f);
				float g = glm::min(finalColor.g * illumination.g, 1.0f);
				float b = glm::min(finalColor.b * illumination.b, 1.0f);

				putPixel(x, y, z, glm::vec3(r,g,b));
			}
		}
	}
}

void Renderer::DrawFaceNormal(std::vector<glm::vec3>& vertices, glm::mat4 m) {
	glm::vec3 p1 = vertices[0],
		p2 = vertices[1],
		p3 = vertices[2],
		mid = (p1 + p2 + p3) / vec3(3),
		normal = glm::cross((p2 - p1), (p3 - p1)),
		startPoint = mid,
		endPoint = mid + (normal * vec3(5));
	int w = GetActiveViewportWidth();
	int h = GetActiveViewportHeight();

	glm::vec4 endPoint4 = Utils::Vec4FromVec3(endPoint),
		startPoint4 = Utils::Vec4FromVec3(startPoint);

	endPoint = Utils::Vec3FromVec4(endPoint4);
	startPoint = Utils::Vec3FromVec4(startPoint4);

	x_add = (int)(w / 2);
	y_add = (int)(h / 2);

	DrawLine(startPoint, endPoint, vec3(1, 0, 0));
}

void Renderer::DrawAxes(const Scene& scene) {
	float size = 400;
	glm::mat4 matrix = Utils::TransMatricesScene(scene);
	glm::vec3
		x_neg = Utils::Mult(matrix, vec3(-size, 0, 0)),
		x_pos = Utils::Mult(matrix, vec3(size, 0, 0)),
		y_neg = Utils::Mult(matrix, vec3(0, -size, 0)),
		y_pos = Utils::Mult(matrix, vec3(0, size, 0)),
		z_neg = Utils::Mult(matrix, vec3(0, 0, -size)),
		z_pos = Utils::Mult(matrix, vec3(0, 0, size));
	
	// x axis
	DrawLine(centerPoint(x_pos), centerPoint(x_neg), vec3(1, 0, 0));

	// y axis
	//DrawLine(centerPoint(y_pos), centerPoint(y_neg), vec3(0, 1, 0));

	// z axis
	DrawLine(centerPoint(z_pos), centerPoint(z_neg), vec3(0, 0, 1));
	
	// optional grid
	for (int i = -size; i <= size; i += size / 8) {
		glm::vec3
			x_s = Utils::Mult(matrix, vec3(i, 0, size)),
			x_e = Utils::Mult(matrix, vec3(i, 0, -size)),
			z_s = Utils::Mult(matrix, vec3(size, 0, i)),
			z_e = Utils::Mult(matrix, vec3(-size, 0, i));
		glm::vec3 color = vec3(0.5, 0.5, 0.5);
		DrawLine(centerPoint(x_s), centerPoint(x_e), color);
		DrawLine(centerPoint(z_s), centerPoint(z_e), color);
	}
}

void Renderer::DrawModel(const Scene& scene, MeshModel* _model, glm::mat4 matrix, bool lightModel) {
	MeshModel model = *_model;
	std::vector<Face> faces = model.GetFaces();
	std::vector<glm::vec3> vertices = model.GetVertices();
	std::vector<glm::vec3> normals = model.GetNormals();

	glm::vec3 min = Utils::Vec3FromVec4(model.GetMin()),
		max = Utils::Vec3FromVec4(model.GetMax());

	for (Face& face : faces) {
		std::vector<glm::vec3> triangle = Utils::FaceToVertices(face, vertices);
		std::vector<glm::vec3> verticesNormals = Utils::FaceToNormals(face, normals);

		for (int i = 0; i < 3; i++) {
			vec3 point = centerPoint(Utils::Mult(matrix, triangle[i]));
			vec3 normal = verticesNormals[i];
			vec3 normalEnd = centerPoint(Utils::Mult(matrix, triangle[i] + (normal * vec3(0.15))));

			triangle[i] = point;
			if (model.showVertexNormals)
				DrawLine(point, normalEnd, vec3(0.5, 0.5, 0));
		}

		DrawTriangle(triangle, verticesNormals, _model, scene, lightModel);
		if (model.showFacesNormals)
			DrawFaceNormal(triangle, matrix);
	}

	if (model.showBoundingBox)
		DrawBoundingBox(matrix, min, max);
}

void Renderer::InitZBuffer() {
	int w = GetActiveViewportWidth();
	int h = GetActiveViewportHeight();
	for (int i = 0; i < w * h; i++)
		zBuffer[i] = INFINITY;
}

void Renderer::Render(const Scene& scene)
{
	//#############################################
	//## You should override this implementation ##
	//## Here you should render the scene.       ##
	//#############################################
	int w = GetActiveViewportWidth();
	int h = GetActiveViewportHeight();
	InitZBuffer();
	x_add = (int)(w / 2);
	y_add = (int)(h / 2);
	fogActivated = scene.fogActivated;
	zNear = scene.GetActiveCamera().n;
	zFar = scene.GetActiveCamera().f;

	centerAxes = vec3(x_add, y_add, 0);

	// draw axes
	DrawAxes(scene);

	std::vector<std::shared_ptr<MeshModel>> models = scene.GetModels();
	std::vector<Camera*> cameras = scene.GetCameras();
	std::vector<Light*> lights = scene.GetLights();

	// draw models
	for (int i = 0; i < scene.GetModelCount(); i++) {
		MeshModel* model = &(*models.at(i));
		(*model).SetWorldTransformation();
		glm::mat4 matrix = Utils::TransMatricesModel(scene, i);
		DrawModel(scene, model, matrix);
	}

	// draw camera models
	for (int i = 0; i < scene.GetCameraCount(); i++) {
		if (i == scene.GetActiveCameraIndex()) continue;
		MeshModel* model = cameras.at(i);
		(*model).SetWorldTransformation();
		glm::mat4 matrix = Utils::TransMatricesCamera(scene, i);
		DrawModel(scene, model, matrix);
	}

	// draw lights
	for (int i = 0; i < scene.GetLightCount(); i++) {
		if (!scene.GetLight(i).isPoint) continue;
		MeshModel* model = &(*lights.at(i));
		(*model).SetWorldTransformation();
		glm::mat4 matrix = Utils::TransMatricesLight(scene, i);
		DrawModel(scene, model, matrix, true);
	}
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void Renderer::SwapBuffers()
{
	float* tempBuffer = colorBuffer;
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	if (alias) {
		tempBuffer = new float[3 * viewportWidth * viewportHeight];
		//InitZBuffer();
		int idx;
		for (int x = 0; x < viewportWidth; x++) {
			for (int y = 0; y < viewportHeight; y++) {
				int nw = INDEX(aliasViewportWidth, 2 * x, 2 * y, 0),
					ne = INDEX(aliasViewportWidth, 2 * x + 1, 2 * y, 0),
					sw = INDEX(aliasViewportWidth, 2 * x, 2 * y + 1, 0),
					se = INDEX(aliasViewportWidth, 2 * x + 1, 2 * y + 1, 0);
				tempBuffer[INDEX(viewportWidth, x, y, 0)] = (colorBuffer[nw + 0] + colorBuffer[ne + 0] + colorBuffer[sw + 0] + colorBuffer[se + 0]) / 4;
				tempBuffer[INDEX(viewportWidth, x, y, 1)] = (colorBuffer[nw + 1] + colorBuffer[ne + 1] + colorBuffer[sw + 1] + colorBuffer[se + 1]) / 4;
				tempBuffer[INDEX(viewportWidth, x, y, 2)] = (colorBuffer[nw + 2] + colorBuffer[ne + 2] + colorBuffer[sw + 2] + colorBuffer[se + 2]) / 4;
			}
		}
	}

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, tempBuffer);

	if (alias) {
		delete[] tempBuffer;
	}

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}