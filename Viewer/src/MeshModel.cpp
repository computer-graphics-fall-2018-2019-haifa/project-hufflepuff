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
	worldTransform(glm::mat4x4(1))
{

}

MeshModel::MeshModel(const MeshModel& other):
	vertices(other.vertices),
	faces(other.faces),
	normals(other.normals),
	modelName(other.modelName),
	worldTransform(other.worldTransform)
{

}

MeshModel::~MeshModel()
{

}

void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	this->worldTransform = worldTransform;
}

void MeshModel::SetWorldTransformation(glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate)
{
	glm::mat4 scaleMat = Utils::GetScaleMatrix(scale);
	glm::mat4 rotateMat = Utils::GetRotationMatrix(rotate);
	glm::mat4 translateMat = Utils::GetTranslationMatrix(translate);
	this->worldTransform = translateMat * rotateMat * scaleMat;
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	return worldTransform;
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
	return modelName;
}

const std::vector<glm::vec3> MeshModel::GetVertices() const
{
	return vertices;
}

const std::vector<Face>& MeshModel::GetFaces() const
{
	return faces;
}

const std::vector<glm::vec3>& MeshModel::GetNormals() const
{
	return normals;
}

