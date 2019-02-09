#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2> textureCoords, const std::string& modelName) :
	vertices(vertices),
	normals(normals),
	faces(faces),
	textureCoords(textureCoords),
	modelName(modelName),
	worldTransform(glm::mat4(1.0f)),
	mins(glm::vec4(glm::vec3(INFINITY), 1.0f)),
	maxs(glm::vec4(glm::vec3(-INFINITY), 1.0f)),
	scale(glm::vec3(1.0f)),
	rotation(glm::vec3(0.0f)),
	translation(glm::vec3(0.0f)),
	location(glm::vec3(0.0f)),
	showFacesNormals(false),
	showVertexNormals(false),
	showBoundingBox(false),
	vao(1),
	vbo(1)
{
	// set a list of model vertices
	modelVertices.reserve(3 * faces.size());
	for (Face face : faces) {
		for (int j = 0; j < 3; j++)
		{
			Vertex vertex;
			int vertexIndex = face.GetVertexIndex(j) - 1;

			vertex.position = vertices[vertexIndex];
			vertex.normal = normals[vertexIndex];

			if (textureCoords.size() > 0)
			{
				int textureCoordsIndex = face.GetTextureIndex(j) - 1;
				vertex.textureCoords = textureCoords[textureCoordsIndex];
			}

			modelVertices.push_back(vertex);
		}
	}

	// set bounding box coords
	for (glm::vec3 vertex : vertices)
	{
		mins.x = std::fmin(mins.x, vertex.x);
		maxs.x = std::fmax(maxs.x, vertex.x);
		mins.y = std::fmin(mins.y, vertex.y);
		maxs.y = std::fmax(maxs.y, vertex.y);
		mins.z = std::fmin(mins.z, vertex.z);
		maxs.z = std::fmax(maxs.z, vertex.z);
	}

	color = Utils::GenerateRandomColor();
	location = translation;

	InitOpenGL();
}

MeshModel::MeshModel(const MeshModel& other) :
	vertices(other.vertices),
	faces(other.faces),
	normals(other.normals),
	textureCoords(other.textureCoords),
	modelName(other.modelName),
	worldTransform(other.worldTransform),
	maxs(other.maxs),
	mins(other.mins),
	scale(other.scale),
	rotation(other.rotation),
	translation(other.translation),
	showFacesNormals(other.showFacesNormals),
	showVertexNormals(other.showVertexNormals),
	showBoundingBox(other.showBoundingBox),
	color(other.color),
	modelVertices(other.modelVertices)
{
	InitOpenGL();
}

MeshModel::~MeshModel()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void MeshModel::InitOpenGL() {
	//GL stuff
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// load vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

	// load normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// load textures
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, textureCoords));

	// unbind
	glBindVertexArray(0);
}

void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	this->worldTransform = worldTransform;
}

void MeshModel::SetWorldTransformation() {
	this->SetWorldTransformation(scale, rotation, translation);
}

void MeshModel::SetWorldTransformation(glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate)
{
	this->worldTransform = Utils::GetTransformationMatrix(scale, rotate, translate);
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	return this->worldTransform;
}

void MeshModel::SetColor(const glm::vec4& color)
{
	this->color = color;
}

const glm::vec4& MeshModel::GetColor() const
{
	return color;
}

const std::string& MeshModel::GetModelName()
{
	return this->modelName;
}

void MeshModel::SetModelName(std::string name)
{
	this->modelName = name;
}

const std::vector<glm::vec3> MeshModel::GetVertices() const
{
	return vertices;
}

const std::vector<Face>& MeshModel::GetFaces() const
{
	return faces;
}

const glm::vec4 MeshModel::GetMin() const
{
	return mins;
}

const glm::vec4 MeshModel::GetMax() const
{
	return maxs;
}

const std::vector<glm::vec3>& MeshModel::GetNormals() const
{
	return normals;
}

const glm::vec3 MeshModel::GetScale() const {
	return scale;
}

const glm::vec3 MeshModel::GetRotation() const {
	return rotation;
}

const glm::vec3 MeshModel::GetTranslation() const {
	return translation;
}

void MeshModel::SetScale(glm::vec3 _scale) {
	scale = _scale;
}

void MeshModel::SetRotation(glm::vec3 _rotation) {
	rotation = _rotation;
}

void MeshModel::SetTranslation(glm::vec3 _translation) {
	translation = _translation;
}

GLuint MeshModel::GetVAO() const
{
	return vao;
}

const std::vector<Vertex>& MeshModel::GetModelVertices()
{
	return modelVertices;
}