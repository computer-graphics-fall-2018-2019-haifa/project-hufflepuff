#include "Utils.h"
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

#define PI 3.14159265358979323846 / 180

glm::vec3 Utils::Vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 Utils::Vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}

MeshModel Utils::LoadMeshModel(const std::string& filePath)
{
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::ifstream ifile(filePath.c_str());

	// while not end of file
	while (!ifile.eof())
	{
		// get line
		std::string curLine;
		std::getline(ifile, curLine);

		// read the type of the line
		std::istringstream issLine(curLine);
		std::string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
		{
			vertices.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vn")
		{
			normals.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vt")
		{
			// Texture coordinates
		}
		else if (lineType == "f")
		{
			faces.push_back(Face(issLine));
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			std::cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}

	return MeshModel(faces, vertices, normals, Utils::GetFileName(filePath));
}

glm::vec4 Utils::Vec4FromVec3(const glm::vec3& v, const float w) {
	return glm::vec4(v, w);
}

glm::vec3 Utils::Vec3FromVec4(const glm::vec4& v, bool divide) {
	glm::vec4 _v = v;
	if (v.w != 0 && divide)
		_v /= v.w;
	return glm::vec3(_v.x, _v.y, _v.z);
}

glm::mat4 Utils::GetTransformationMatrix(glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate) {
	glm::mat4 scaleMat = GetScaleMatrix(scale);
	glm::mat4 rotateMat = GetRotationMatrix(rotate);
	glm::mat4 translateMat = GetTranslationMatrix(translate);

	return
		glm::transpose(translateMat)
		* rotateMat
		* scaleMat;
}


glm::mat4 Utils::GetScaleMatrix(const glm::vec3 scaleVector) {
	float x = scaleVector.x,
		y = scaleVector.y,
		z = scaleVector.z;

	return glm::mat4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	);
}

glm::mat4 Utils::GetTranslationMatrix(const glm::vec3 translationVector) {
	float x = translationVector.x,
		y = translationVector.y,
		z = translationVector.z;

	return glm::mat4(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	);
}

glm::mat4 Utils::GetRotationMatrix(const glm::vec3 rotateVector) {
	glm::mat3 xAxis, yAxis, zAxis;
	float x = rotateVector.x * PI,
		y = rotateVector.y * PI,
		z = rotateVector.z * PI;

	xAxis = glm::mat4(
		1, 0, 0, 0,
		0, cos(x), -sin(x), 0,
		0, sin(x),  cos(x), 0,
		0, 0, 0, 1
	);
	yAxis = glm::mat4(
		cos(y), 0,  sin(y), 0,
		0, 1, 0, 0,
		-sin(y), 0, cos(y), 0,
		0, 0, 0, 1
	);
	zAxis = glm::mat4(
		cos(z), -sin(z), 0, 0,
		sin(z), cos(z),  0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	return xAxis * yAxis * zAxis;
}

glm::mat4 Utils::TransMatricesScene(const Scene & scene) {
	Camera camera = scene.GetActiveCamera();

	glm::mat4
		swtMat = scene.GetWorldTransformation(),
		cvtMat = camera.GetViewTransformation(),
		cptMat = camera.GetProjTransformation();

	return cptMat * cvtMat * swtMat;
}

glm::mat4 Utils::TransMatricesModel(const Scene & scene, int modelIdx) {
	glm::mat4 wtMat = scene.GetModel(modelIdx).GetWorldTransformation();
	return TransMatricesScene(scene) * wtMat;
}

glm::mat4 Utils::TransMatricesCamera(const Scene & scene, int cameraIdx) {
	glm::mat4 wtMat = scene.GetCamera(cameraIdx).GetWorldTransformation();
	return TransMatricesScene(scene) * wtMat;
}

glm::vec3 Utils::Mult(glm::mat4 & mat, glm::vec3 & point)
{
	return Vec3FromVec4(mat * Vec4FromVec3(point));
}

glm::vec3 Utils::Mult(glm::mat4 & mat, glm::vec4 & point)
{
	return Vec3FromVec4(mat * point);
}

std::vector<glm::vec3> Utils::FaceToVertices(const Face & face, const std::vector<glm::vec3>& vertices)
{
	int i = face.GetVertexIndex(0),
		j = face.GetVertexIndex(1),
		k = face.GetVertexIndex(2);
	std::vector<glm::vec3> v = std::vector<glm::vec3>();

	v.push_back(vertices[i - 1]);
	v.push_back(vertices[j - 1]);
	v.push_back(vertices[k - 1]);
	return v;
}

std::vector<glm::vec3> Utils::FaceToNormals(const Face & face, const std::vector<glm::vec3>& normals)
{
	int i = face.GetNormalIndex(0),
		j = face.GetNormalIndex(1),
		k = face.GetNormalIndex(2);
	std::vector<glm::vec3> v = std::vector<glm::vec3>();

	v.push_back(normals[i - 1]);
	v.push_back(normals[j - 1]);
	v.push_back(normals[k - 1]);
	return v;
}

glm::vec4 Utils::GenerateRandomColor()
{
	glm::vec4 color(0);
	color.x = static_cast<float>(rand()) / (RAND_MAX / 2);
	color.y = static_cast<float>(rand()) / (RAND_MAX / 2);
	color.z = static_cast<float>(rand()) / (RAND_MAX / 2);
	color.w = 1.0f;
	return color;
}

std::string Utils::GetFileName(const std::string& filePath)
{
	if (filePath.empty()) {
		return {};
	}

	auto len = filePath.length();
	auto index = filePath.find_last_of("/\\");

	if (index == std::string::npos) {
		return filePath;
	}

	if (index + 1 >= len) {

		len--;
		index = filePath.substr(0, len).find_last_of("/\\");

		if (len == 0) {
			return filePath;
		}

		if (index == 0) {
			return filePath.substr(1, len - 1);
		}

		if (index == std::string::npos) {
			return filePath.substr(0, len);
		}

		return filePath.substr(index + 1, len - index - 1);
	}

	return filePath.substr(index + 1, len - index);
}