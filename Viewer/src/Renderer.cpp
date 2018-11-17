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

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)

typedef glm::vec3 vec3;
int x_add;
int y_add;

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr)
{
	initOpenGLRendering();
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

void Renderer::putPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewportWidth) return;
	if (j < 0) return; if (j >= viewportHeight) return;
	colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x;
	colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
	colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
}

void Renderer::createBuffers(int viewportWidth, int viewportHeight)
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}

	colorBuffer = new float[3* viewportWidth * viewportHeight];
	for (int x = 0; x < viewportWidth; x++)
	{
		for (int y = 0; y < viewportHeight; y++)
		{
			putPixel(x, y, glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewportWidth; i++)
	{
		for (int j = 0; j < viewportHeight; j++)
		{
			putPixel(i, j, color);
		}
	}
}

void Renderer::SetViewport(int viewportWidth, int viewportHeight, int viewportX, int viewportY)
{
	this->viewportX = viewportX;
	this->viewportY = viewportY;
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	createBuffers(viewportWidth, viewportHeight);
	createOpenGLBuffer();

	int x_add = (int)(viewportWidth / 2);
	int y_add = (int)(viewportHeight / 2);

	centerAxes = vec3(x_add, y_add, 0);
}

glm::vec3 Renderer::centerPoint(glm::vec3 point)
{
	return point + centerAxes;
}

glm::vec3 Renderer::centerPoint(glm::vec4 point)
{
	return Utils::Vec3FromVec4(point) + centerAxes;
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

	for (int x = (int)x1; x<maxX; x++)
	{
		if (steep)
		{
			putPixel(y, x, color);
		}
		else
		{
			putPixel(x, y, color);
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

void Renderer::DrawTriangle(std::vector<glm::vec3>& vertices, glm::vec3& color) {
	glm::vec3 p1 = vertices[0],
		p2 = vertices[1],
		p3 = vertices[2];
	DrawLine(p1, p2, color);
	DrawLine(p1, p3, color);
	DrawLine(p2, p3, color);
}

void Renderer::DrawFaceNormal(std::vector<glm::vec3>& vertices, glm::mat4 m) {
	glm::vec3 p1 = vertices[0],
		p2 = vertices[1],
		p3 = vertices[2],
		mid = (p1 + p2 + p3) / vec3(3),
		normal = glm::cross((p2 - p1), (p3 - p1)),
		startPoint = mid - (normal * vec3(1)),
		endPoint = mid + (normal * vec3(1));

	glm::vec4 endPoint4 = Utils::Vec4FromVec3(endPoint),
		startPoint4 = Utils::Vec4FromVec3(startPoint);

	endPoint = Utils::Vec3FromVec4(endPoint4);
	startPoint = Utils::Vec3FromVec4(startPoint4);

	x_add = (int)(viewportWidth / 2);
	y_add = (int)(viewportHeight / 2);

	DrawLine(startPoint, endPoint, vec3(0.5, 0, 0.5));
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

	// optional grid
	for (int i = -size; i <=  size; i += size / 8) {
		glm::vec3
			x_s = Utils::Mult(matrix, vec3(i, 0, size)),
			x_e = Utils::Mult(matrix, vec3(i, 0, -size)),
			z_s = Utils::Mult(matrix, vec3(size, 0, i)),
			z_e = Utils::Mult(matrix, vec3(-size, 0, i));
		glm::vec3 color = vec3(0.5, 0.5, 0.5);
		DrawLine(centerPoint(x_s), centerPoint(x_e), color);
		DrawLine(centerPoint(z_s), centerPoint(z_e), color);
	}

	// x axis
	DrawLine(centerPoint(x_pos), centerPoint(x_neg), vec3(1, 0, 0));

	// y axis
	DrawLine(centerPoint(y_pos), centerPoint(y_neg), vec3(0, 1, 0));

	// z axis
	DrawLine(centerPoint(z_pos), centerPoint(z_neg), vec3(0, 0, 1));
}

void Renderer::DrawModel(MeshModel* _model, glm::mat4 matrix) {
	MeshModel model = *_model;
	std::vector<Face> faces = model.GetFaces();
	std::vector<glm::vec3> vertices = model.GetVertices();
	std::vector<glm::vec3> normals = model.GetNormals();
	vec3 modelColor = Utils::Vec3FromVec4(model.color, false);

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

		DrawTriangle(triangle, modelColor);
		if (model.showFacesNormals)
			DrawFaceNormal(triangle, matrix);
	}

	if (model.showBoundingBox)
		DrawBoundingBox(matrix, min, max);
}

void Renderer::Render(const Scene& scene)
{
	//#############################################
	//## You should override this implementation ##
	//## Here you should render the scene.       ##
	//#############################################

	x_add = (int)(viewportWidth / 2);
	y_add = (int)(viewportHeight / 2);

	centerAxes = vec3(x_add, y_add, 0);

	// draw axes
	DrawAxes(scene);

	std::vector<std::shared_ptr<MeshModel>> models = scene.GetModels();
	std::vector<Camera*> cameras = scene.GetCameras();

	// draw models
	for (int i = 0; i < scene.GetModelCount(); i++) {
		MeshModel* model = &(*models.at(i));
		(*model).SetWorldTransformation();
		glm::mat4 matrix = Utils::TransMatricesModel(scene, i);
		DrawModel(model, matrix);
	}

	// draw camera models
	for (int i = 0; i < scene.GetCameraCount(); i++) {
		if (i == scene.GetActiveCameraIndex()) continue;
		MeshModel* model = cameras.at(i);
		(*model).SetWorldTransformation();
		glm::mat4 matrix = Utils::TransMatricesCamera(scene, i);
		DrawModel(model, matrix);
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
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, colorBuffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}