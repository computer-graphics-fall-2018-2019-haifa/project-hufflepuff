#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	vertices(vertices),
	normals(normals),
	faces(faces),
	modelName(modelName),
	worldTransform(glm::mat4x4(1)),
	mins(glm::vec4(INFINITY, INFINITY, INFINITY, 1)),
	maxs(glm::vec4(-INFINITY, -INFINITY, -INFINITY, 1)),
	scale({ 1,1,1 }),
	rotation({ 0,0,0 }),
	translation({ 0,0,0 }),
	avg({ 0,0,0 }),
	showFacesNormals(false),
	showVertexNormals(false),
	showBoundingBox(false)
{
	for (glm::vec3 vertex : vertices)
	{
		mins.x = std::fmin(mins.x, vertex.x);
		maxs.x = std::fmax(maxs.x, vertex.x);
		mins.y = std::fmin(mins.y, vertex.y);
		maxs.y = std::fmax(maxs.y, vertex.y);
		mins.z = std::fmin(mins.z, vertex.z);
		maxs.z = std::fmax(maxs.z, vertex.z);
		avg.x += vertex.x;
		avg.y += vertex.y;
		avg.z += vertex.z;
	}

	color = Utils::GenerateRandomColor();
	avg /= glm::vec3(vertices.size());
}

MeshModel::MeshModel(const MeshModel& other):
	vertices(other.vertices),
	faces(other.faces),
	normals(other.normals),
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
	color(other.color)
{
}

MeshModel::~MeshModel()
{

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

