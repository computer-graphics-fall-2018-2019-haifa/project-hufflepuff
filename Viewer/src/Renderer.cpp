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

void Renderer::DrawSquare(vec3 vertices[4]) {
	vec3 a = vertices[0],
		b = vertices[1],
		c = vertices[2],
		d = vertices[3],
		red = vec3(1, 0, 0);
	DrawLine(a, b, red);
	DrawLine(a, d, red);
	DrawLine(b, c, red);
	DrawLine(c, d, red);
}

void Renderer::DrawBoundingBox(const glm::vec4& min, const glm::vec4& max)
{
	glm::vec3 min3 = Utils::Vec3FromVec4(min);
	glm::vec3 max3 = Utils::Vec3FromVec4(max);

	min3 += vec3(x_add, y_add, 0);
	max3 += vec3(x_add, y_add, 0);

	vec3 lowerSquare[4] = {
		glm::vec3(min3.x, max3.y, min3.z),
		glm::vec3(max3.x, max3.y, min3.z),
		glm::vec3(max3.x, min3.y, max3.z),
		glm::vec3(min3.x, min3.y, max3.z)
	};
	DrawSquare(lowerSquare);

	////Upper Square
	//DrawLine(
	//	glm::vec3(min3.x, max3.y, min3.z),
	//	glm::vec3(max3.x, max3.y, min3.z),
	//	glm::vec3(1, 0, 0)
	//);
	//
	//DrawLine(
	//	glm::vec3(min3.x, max3.y, min3.z),
	//	glm::vec3(min3.x, max3.y, max3.z),
	//	glm::vec3(1, 0, 0)
	//);
	//
	//DrawLine(
	//	glm::vec3(min3.x, max3.y, max3.z),
	//	glm::vec3(max3.x, max3.y, max3.z),
	//	glm::vec3(1, 0, 0)
	//);
	//
	//DrawLine(
	//	glm::vec3(max3.x, max3.y, max3.z),
	//	glm::vec3(max3.x, max3.y, min3.z),
	//	glm::vec3(1, 0, 0)
	//);
	//

	////Pillars

	//DrawLine(
	//	glm::vec3(min3.x, min3.y, min3.z),
	//	glm::vec3(min3.x, max3.y, min3.z),
	//	glm::vec3(1, 0, 0)
	//);
	//

	//DrawLine(
	//	glm::vec3(max3.x, min3.y, min3.z),
	//	glm::vec3(max3.x, max3.y, min3.z),
	//	glm::vec3(1, 0, 0)
	//);
	//
	//DrawLine(
	//	glm::vec3(max3.x, min3.y, max3.z),
	//	glm::vec3(max3.x, max3.y, max3.z),
	//	glm::vec3(1, 0, 0)
	//);
	//
	//DrawLine(
	//	glm::vec3(min3.x, min3.y, max3.z),
	//	glm::vec3(min3.x, max3.y, max3.z),
	//	glm::vec3(1, 0, 0)
	//);
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
		midp2p3 = p2 + p3 / vec3(2,2,2),
		normal = glm::cross((p2 - p1), (p3 - p1)),
		startPoint = p1 - (normal * vec3(0.25, 0.25, 0.25)),
		endPoint = p1 + (normal * vec3(0.25, 0.25, 0.25));

	glm::vec4 endPoint4 = Utils::Vec4FromVec3(endPoint),
		startPoint4 = Utils::Vec4FromVec3(startPoint);

	endPoint = Utils::Vec3FromVec4(endPoint4);
	startPoint = Utils::Vec3FromVec4(startPoint4);

	x_add = (int)(viewportWidth / 2);
	y_add = (int)(viewportHeight / 2);

	//endPoint += vec3(x_add, y_add, 0);

	DrawLine(startPoint, endPoint, vec3(1, 0, 0));
}

void Renderer::Render(const Scene& scene)
{
	//#############################################
	//## You should override this implementation ##
	//## Here you should render the scene.       ##
	//#############################################

	x_add = (int)(viewportWidth / 2);
	y_add = (int)(viewportHeight / 2);

	// draw axes
	//DrawLine(vec3(0, y_add, 0), vec3(viewportWidth, y_add, 0), vec3(1, 1, 0));
	//DrawLine(vec3(x_add, 0, 0), vec3(x_add, viewportHeight, 0), vec3(1, 1, 0));

	int modelIndex = scene.GetActiveModelIndex();

	Camera c = scene.GetActiveCamera();
	glm::mat4 vtMat = c.GetViewTransformation();
	glm::mat4 swtMat = scene.GetWorldTransformation();

	if (c.isOrth)
		c.SetOrthographicProjection(1, 1, 10, 150) ;
	else
		c.SetPerspectiveProjection(45, 1, 100, 1000);
	glm::mat4 ptMat = c.GetProjTransformation();

	std::vector<std::shared_ptr<MeshModel>> models = scene.GetModels();
	if (models.empty()) return;	

	for (std::shared_ptr<MeshModel>& modelPointer : models) {
		MeshModel m = *modelPointer;
		std::vector<Face> faces = m.GetFaces();
		std::vector<glm::vec3> vertices = m.GetVertices();
		std::vector<glm::vec3> normals = m.GetNormals();

		vec3 scaleVec3 = m.GetScale();
		vec3 rotateVec3 = m.GetRotation();
		vec3 translationVec3 = m.GetTranslation();
		vec3 centerVec3 = vec3(x_add, y_add, 0);
		m.SetWorldTransformation(scaleVec3, rotateVec3, translationVec3);
		glm::mat4 wtMat = m.GetWorldTransformation();

		glm::vec4 min = m.GetMin();
		glm::vec4 max = m.GetMax();

		min = ptMat * vtMat * swtMat * wtMat * min;
		max = ptMat * vtMat * swtMat * wtMat * max;

		glm::vec3 modelColor = Utils::Vec3FromVec4(m.color, false);

		for (Face& face : faces) {
			std::vector<glm::vec3> triangle = Utils::FaceToVertices(face, vertices);
			std::vector<glm::vec3> verticesNormals = Utils::FaceToNormals(face, normals);

			for (int i = 0; i < 3; i++) {
				glm::vec3 point = triangle[i];
				glm::vec3 normal = verticesNormals[i];
				glm::vec3 normalEnd = point + (normal * vec3(0.25, 0.25, 0.25));

				glm::vec4 pointVec4 = Utils::Vec4FromVec3(point);
				glm::vec4 normalVec4 = Utils::Vec4FromVec3(normalEnd);

				//glm::vec4 aspect(viewportWidth / viewportHeight, 1, 1, 1);

				pointVec4 =  ptMat * vtMat * swtMat * wtMat * pointVec4;
				normalVec4 = ptMat * vtMat * swtMat * wtMat * normalVec4;

				point = Utils::Vec3FromVec4(pointVec4);
				normalEnd = Utils::Vec3FromVec4(normalVec4);

				//point += vec3(x_add, y_add, 0);
				//normalEnd += vec3(x_add, y_add, 0);

				triangle[i] = point;
				if (m.showVertexNormals)
					DrawLine(point, normalEnd, vec3(0, 1, 0));
			}

			DrawTriangle(triangle, modelColor);
			if (m.showFacesNormals)
				DrawFaceNormal(triangle, ptMat * vtMat * swtMat * wtMat);
		}
		if (m.showBoundingBox)
			DrawBoundingBox(min, max);
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