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

vec3 rotateHelper(vec3 &point, const float &theta);
vec3 scaleHelper(vec3& point, vec3& scale);


Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr)
{
	initOpenGLRendering();
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);
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
}

void Renderer::DrawLine(const vec3& point1, const vec3& point2, const vec3& color) {
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

void Renderer::DrawTriangle(std::vector<glm::vec3>& vertices, glm::vec3& color) {
	glm::vec3 p1 = vertices[0],
		p2 = vertices[1],
		p3 = vertices[2];
	DrawLine(p1, p2, color);
	DrawLine(p1, p3, color);
	DrawLine(p2, p3, color);
}

void Renderer::Render(const Scene& scene, const float& scale, const float& rotation, const float translate[2])
{
	//#############################################
	//## You should override this implementation ##
	//## Here you should render the scene.       ##
	//#############################################

	int x_add = (int)(viewportWidth / 2);
	int y_add = (int)(viewportHeight / 2);

	// draw axes
	DrawLine(vec3(0, y_add, 0), vec3(viewportWidth, y_add, 0), vec3(1, 1, 0));
	DrawLine(vec3(x_add, 0, 0), vec3(x_add, viewportHeight, 0), vec3(1, 1, 0));

	int modelIndex = scene.GetActiveModelIndex();

	Camera c = scene.GetActiveCamera();
	glm::mat4 vtMat = c.GetViewTransformation();
	glm::mat4 ptMat = c.GetPersTransformation();
	glm::mat4 otMat = c.GetOrthTransformation();

	std::vector<std::shared_ptr<MeshModel>> models = scene.GetModels();
	if (models.empty()) return;
	MeshModel m = *models.at(modelIndex);
	std::vector<Face> faces = m.GetFaces();
	std::vector<glm::vec3> vertices = m.GetVertices();
	std::vector<glm::vec3> normals = m.GetNormals();

	float trans_x = translate[0],
		trans_y = translate[1];
	vec3 scaleVec3 = vec3(scale, scale, scale);
	vec3 rotateVec3 = vec3(0, rotation, 0);
	vec3 translationVec3 = vec3(trans_x, trans_y, 0);
	vec3 centerVec3 = vec3(x_add, y_add, 0);
	m.SetWorldTransformation(scaleVec3, rotateVec3, translationVec3);

	glm::mat4 wtMat = m.GetWorldTransformation();

	for (Face& face : faces) {
		std::vector<glm::vec3> triangle = Utils::FaceToVertices(face, vertices);

		for (glm::vec3& point : triangle) {
			glm::vec4 pointVec4 = Utils::Vec4FromVec3(point);
			pointVec4 = ptMat * otMat * vtMat * wtMat * pointVec4;
			point = Utils::Vec3FromVec4(pointVec4);
			point += vec3(x_add, y_add, 0);
		}
		
		DrawTriangle(triangle, glm::vec3(0, 0, 0));
	}
}

vec3 scaleHelper(vec3& point, vec3& scale) {
	return Utils::GetScaleMatrix(scale) * Utils::Vec4FromVec3(point);
}

vec3 rotateHelper(vec3& point, const float& theta) {
	glm::mat3x3 rotationMatrix;
	const float pi = 3.14159265 / 180;
	rotationMatrix[0][0] = cos(theta * pi);
	rotationMatrix[0][1] = -sin(theta * pi);
	rotationMatrix[1][0] = sin(theta * pi);
	rotationMatrix[1][1] = cos(theta * pi);
	rotationMatrix[2][2] = 1;

	return rotationMatrix * point;
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