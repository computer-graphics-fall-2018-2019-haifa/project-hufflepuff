#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

float PI = 3.14159265359f;

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2> textureCoords, const std::string& modelName) :
	vertices(vertices),
	normals(normals),
	faces(faces),
	textureCoords(textureCoords),
	modelName(modelName),
	worldTransform(glm::mat4(1.0f)),
	mins(glm::vec4(glm::vec3(INFINITY), 1.0f)),
	maxs(glm::vec4(glm::vec3(-INFINITY), 1.0f)),
	avg(glm::vec3(0)),
	scale(glm::vec3(1.0f)),
	rotation(glm::vec3(0.0f)),
	translation(glm::vec3(0.0f)),
	location(glm::vec3(0.0f)),
	showFacesNormals(false),
	showVertexNormals(false),
	showBoundingBox(false),
	showWire(false),
	useTexture(false),
	loadedTexture(false),
	fill(true),
	vao(1),
	vbo(1),
	Ka(0.5f),
	Kd(0.7f),
	Ks(0.2f),
	alpha(3.0f)
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
			else {
				vertex.textureCoords = glm::vec2(vertex.position);
			}

			modelVertices.push_back(vertex);
		}
	}

	color = Utils::GenerateRandomColor();
	location = translation;

	PopulateBoundingBoxVertices();
	PopulateVertexNormals();

	InitOpenGL(&vao, &vbo, modelVertices);
	InitOpenGL(&boxVao, &boxVbo, boundingBoxVertices);
	InitOpenGL(&normalVao, &normalVbo, vertexNormals);
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
	avg(other.avg),
	scale(other.scale),
	rotation(other.rotation),
	translation(other.translation),
	showFacesNormals(other.showFacesNormals),
	showVertexNormals(other.showVertexNormals),
	showBoundingBox(other.showBoundingBox),
	showWire(other.showWire),
	fill(other.fill),
	color(other.color),
	modelVertices(other.modelVertices),
	boundingBoxVertices(other.boundingBoxVertices),
	vertexNormals(other.vertexNormals),
	Ka(other.Ka),
	Kd(other.Kd),
	Ks(other.Ks),
	alpha(other.alpha),
	useTexture(other.useTexture),
	texture(other.texture),
	loadedTexture(other.loadedTexture)
{
	InitOpenGL(&vao, &vbo, modelVertices);
	InitOpenGL(&boxVao, &boxVbo, boundingBoxVertices);
	InitOpenGL(&normalVao, &normalVbo, vertexNormals);
}

MeshModel::~MeshModel()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void MeshModel::InitOpenGL(GLuint* vao, GLuint* vbo, std::vector<Vertex>& vertices) {
	//GL stuff
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

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

void MeshModel::UpdateModelVerticesData(std::vector<Vertex>& newVertices) {
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, newVertices.size() * sizeof(Vertex), &newVertices[0]);

	glBindVertexArray(0);
}

void MeshModel::LoadBombingTexture() {
	loadedTexture = true;
	useTexture = true;
	texture.generateBombingTexture(true);
}

void MeshModel::ChangeTextureProjection(int type) {
	std::vector<Vertex> newVertices(modelVertices);
	
	if (type == ORIGINAL) {
	} else if (type == PLANAR) {
		for (Vertex& v : newVertices) {
			v.textureCoords = glm::vec2(v.position.x, v.position.z);
		}
	}
	else if (type == SPHERICAL) {
		for (Vertex& v : newVertices) {
			glm::vec2 v2 = glm::vec2(glm::atan(v.position.x, v.position.y) / 2 * PI + 0.5f, 0.5f - glm::asin(v.position.z) / PI);
			v.textureCoords = glm::normalize(v2);
		}
	}
	else if (type == CYLINDRICAL) {
		float radius = 5.0;
		for (Vertex& v : newVertices) {
			float theta = glm::atan(v.position.z / v.position.x);
			v.textureCoords.x = radius * glm::cos(2 * PI * theta);
			v.textureCoords.y = radius * glm::sin(2 * PI * theta);
			//v.textureCoords = glm::normalize(glm::abs(glm::vec2(glm::cos(theta), glm::sin(theta))));
		}
	}

	UpdateModelVerticesData(newVertices);
}

void MeshModel::PopulateBoundingBoxVertices() {
	// set bounding box coords
	for (glm::vec3 vertex : vertices)
	{
		mins.x = std::fmin(mins.x, vertex.x);
		maxs.x = std::fmax(maxs.x, vertex.x);
		mins.y = std::fmin(mins.y, vertex.y);
		maxs.y = std::fmax(maxs.y, vertex.y);
		mins.z = std::fmin(mins.z, vertex.z);
		maxs.z = std::fmax(maxs.z, vertex.z);
		avg += vertex;
	}
	avg /= vertices.size();

	Vertex ftl, ftr, fbl, fbr, btl, btr, bbl, bbr;
	ftl.position = ftl.normal = glm::vec3(mins.x, maxs.y, mins.z);
	ftr.position = ftr.normal = glm::vec3(maxs.x, maxs.y, mins.z);
	fbl.position = fbl.normal = glm::vec3(mins.x, mins.y, mins.z);
	fbr.position = fbr.normal = glm::vec3(maxs.x, mins.y, mins.z);
	btl.position = btl.normal = glm::vec3(mins.x, maxs.y, maxs.z);
	btr.position = btr.normal = glm::vec3(maxs.x, maxs.y, maxs.z);
	bbl.position = bbl.normal = glm::vec3(mins.x, mins.y, maxs.z);
	bbr.position = bbr.normal = glm::vec3(maxs.x, mins.y, maxs.z);
	ftl.textureCoords = glm::vec2(0.0f, 0.0f);
	ftr.textureCoords = glm::vec2(0.0f, 0.0f);
	fbl.textureCoords = glm::vec2(0.0f, 0.0f);
	fbr.textureCoords = glm::vec2(0.0f, 0.0f);
	btl.textureCoords = glm::vec2(0.0f, 0.0f);
	btr.textureCoords = glm::vec2(0.0f, 0.0f);
	bbl.textureCoords = glm::vec2(0.0f, 0.0f);
	bbr.textureCoords = glm::vec2(0.0f, 0.0f);

	boundingBoxVertices.reserve(36);
	boundingBoxVertices.push_back(fbl); boundingBoxVertices.push_back(fbr); boundingBoxVertices.push_back(ftr);
	boundingBoxVertices.push_back(fbl); boundingBoxVertices.push_back(fbr); boundingBoxVertices.push_back(ftl);
	boundingBoxVertices.push_back(fbr); boundingBoxVertices.push_back(ftr); boundingBoxVertices.push_back(bbr);
	boundingBoxVertices.push_back(fbr); boundingBoxVertices.push_back(bbr); boundingBoxVertices.push_back(btr);
	boundingBoxVertices.push_back(bbl); boundingBoxVertices.push_back(bbr); boundingBoxVertices.push_back(btl);
	boundingBoxVertices.push_back(bbr); boundingBoxVertices.push_back(btl); boundingBoxVertices.push_back(btr);
	boundingBoxVertices.push_back(fbl); boundingBoxVertices.push_back(ftl); boundingBoxVertices.push_back(bbl);
	boundingBoxVertices.push_back(ftl); boundingBoxVertices.push_back(bbl); boundingBoxVertices.push_back(btl);
	boundingBoxVertices.push_back(ftl); boundingBoxVertices.push_back(ftr); boundingBoxVertices.push_back(btr);
	boundingBoxVertices.push_back(ftl); boundingBoxVertices.push_back(btl); boundingBoxVertices.push_back(btr);
	boundingBoxVertices.push_back(fbl); boundingBoxVertices.push_back(fbr); boundingBoxVertices.push_back(bbr);
	boundingBoxVertices.push_back(fbl); boundingBoxVertices.push_back(bbl); boundingBoxVertices.push_back(bbr);
}

void MeshModel::PopulateVertexNormals() {
	vertexNormals.reserve(2 * vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		Vertex start, end;
		start.position = vertices[i];
		start.normal = normals[i];
		start.textureCoords = glm::vec2(0, 0);

		end = start;
		end.position += glm::vec3(0.1) * start.normal;

		vertexNormals.push_back(start);
		vertexNormals.push_back(end);
	}
}

std::vector<Vertex>& MeshModel::GetBoundingBoxVertices()
{
	return boundingBoxVertices;
}

std::vector<Vertex>& MeshModel::GetVertexNormals()
{
	return vertexNormals;
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

void MeshModel::LoadTexture(const char * path) {
	texture.loadTexture(path, true);
	loadedTexture = true;
	useTexture = true;
}

void MeshModel::BindTexture() {
	texture.bind(0);
}

void MeshModel::UnbindTexture()
{
	texture.unbind(0);
}
