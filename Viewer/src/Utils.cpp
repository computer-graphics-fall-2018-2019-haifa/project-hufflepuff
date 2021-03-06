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
	std::vector<glm::vec2> textureCoords;
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
			textureCoords.push_back(Utils::Vec2fFromStream(issLine));
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
			// std::cout << "Found unknown line Type \"" << lineType << "\"" << std::endl;
		}
	}

	return MeshModel(faces, vertices, CalculateNormals(vertices, faces), textureCoords, Utils::GetFileName(filePath));
}

std::vector<glm::vec3> Utils::CalculateNormals(std::vector<glm::vec3> vertices, std::vector<Face> faces)
{
	std::vector<glm::vec3> normals(vertices.size());
	std::vector<int> adjacent_faces_count(vertices.size());

	for (int i = 0; i < adjacent_faces_count.size(); i++)
	{
		adjacent_faces_count[i] = 0;
	}

	for (int i = 0; i < faces.size(); i++)
	{
		Face currentFace = faces.at(i);

		int index0 = currentFace.GetVertexIndex(0) - 1;
		int index1 = currentFace.GetVertexIndex(1) - 1;
		int index2 = currentFace.GetVertexIndex(2) - 1;

		glm::vec3 v0 = vertices.at(index0);
		glm::vec3 v1 = vertices.at(index1);
		glm::vec3 v2 = vertices.at(index2);

		glm::vec3 u = v0 - v1;
		glm::vec3 v = v2 - v1;
		glm::vec3 face_normal = glm::normalize(-glm::cross(u, v));

		normals.at(index0) += face_normal;
		normals.at(index1) += face_normal;
		normals.at(index2) += face_normal;

		adjacent_faces_count.at(index0) += 1;
		adjacent_faces_count.at(index1) += 1;
		adjacent_faces_count.at(index2) += 1;
	}

	for (int i = 0; i < normals.size(); i++)
	{
		normals[i] /= adjacent_faces_count[i];
		normals[i] = glm::normalize(normals[i]);
	}

	return normals;
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

glm::vec2 Utils::GetBarycentricCoords(glm::vec3 p, std::vector<glm::vec3> vertices) {
	glm::vec3 v0 = vertices[0],
		v1 = vertices[1],
		v2 = vertices[2];
	glm::vec2 u = v1 - v0;
	glm::vec2 v = v2 - v0;
	glm::vec2 w = p - v0;

	double lambda1 = (w.y * v.x - v.y * w.x) / (u.y * v.x - u.x * v.y);
	double lambda2 = (w.y - lambda1 * u.y) / v.y;

	return glm::vec2(lambda1, lambda2);
}

bool Utils::InTriangle(glm::vec2 bar) {
	double x = bar.x,
		y = bar.y;
	return x >= 0 && y >= 0 && (x + y) <= 1;
}

glm::vec3 Utils::CalcFaceNormal(std::vector<glm::vec3> vertices) {
	glm::vec3 p1 = vertices[0],
		p2 = vertices[1],
		p3 = vertices[2];
	return glm::normalize(glm::cross((p2 - p1), (p3 - p1)));
}

glm::mat4 Utils::GetTransformationMatrix(glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate) {
	glm::mat4 scaleMat = GetScaleMatrix(scale);
	glm::mat4 rotateMat = GetRotationMatrix(rotate);
	glm::mat4 translateMat = glm::transpose(GetTranslationMatrix(translate));

	return translateMat * rotateMat * scaleMat;
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
		cvtMat = camera.GetViewTransformation(),
		cptMat = camera.GetProjTransformation();

	return cptMat * cvtMat;
}

glm::mat4 Utils::TransMatricesModel(const Scene & scene, int modelIdx) {
	glm::mat4 wtMat = scene.GetModel(modelIdx).GetWorldTransformation();
	glm::mat4 swtMat = scene.GetWorldTransformation();
	return TransMatricesScene(scene) * swtMat * wtMat;
}

glm::mat4 Utils::TransMatricesLight(const Scene & scene, int lightIdx) {
	glm::mat4 wtMat = scene.GetLight(lightIdx).GetWorldTransformation();
	glm::mat4 swtMat = scene.GetWorldTransformation();
	return TransMatricesScene(scene) * swtMat * wtMat;
}

glm::mat4 Utils::TransMatricesCamera(const Scene & scene, int cameraIdx) {
	glm::mat4 wtMat = scene.GetCamera(cameraIdx).GetWorldTransformation();
	glm::mat4 swtMat = scene.GetWorldTransformation();
	return TransMatricesScene(scene) * swtMat * wtMat;
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

glm::vec3 Utils::GetMarbleColor(float val, glm::vec3 c1, glm::vec3 c2) {
	float x = glm::clamp(val, 0.0f, 1.0f);
	return glm::mix(c1, c2, x);
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
	color.x = (static_cast<float>(rand()) / (RAND_MAX));
	color.y = (static_cast<float>(rand()) / (RAND_MAX));
	color.z = (static_cast<float>(rand()) / (RAND_MAX));
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