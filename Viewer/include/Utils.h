#pragma once
#include <glm/glm.hpp>
#include <string>
#include "MeshModel.h"
#include "Scene.h"


/*
 * Utils class.
 * This class is consisted of static helper methods that can have many clients across the code.
 */
class Utils
{
public:
	static glm::vec3 Vec3fFromStream(std::istream& issLine);
	static glm::vec2 Vec2fFromStream(std::istream& issLine);
	static MeshModel LoadMeshModel(const std::string& filePath);

	// Add here more static utility functions...
	// For example:
	//	1. function that gets an angle, and returns a rotation matrix around a certian axis
	//	2. function that gets a vector, and returns a translation matrix
	//	3. anything else that may be useful in global scope
	static glm::vec4 Vec4FromVec3(const glm::vec3& v, const float w = 1.0f);
	static glm::vec3 Vec3FromVec4(const glm::vec4& v, bool divide = true);

	static glm::mat4 GetTransformationMatrix(glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate);
	static glm::mat4 GetScaleMatrix(const glm::vec3 s);
	static glm::mat4 GetTranslationMatrix(const glm::vec3 translationVector);
	static glm::mat4 GetRotationMatrix(const glm::vec3 rotateVector);

	static glm::mat4 TransMatricesScene(const Scene & scene);
	static glm::mat4 TransMatricesModel(const Scene & scene, int modelIdx = -1);
	static glm::mat4 TransMatricesCamera(const Scene & scene, int cameraIdx = -1);

	static glm::vec3 Mult(glm::mat4& mat, glm::vec3& point);
	static glm::vec3 Mult(glm::mat4& mat, glm::vec4& point);

	static std::vector<glm::vec3> FaceToVertices(const Face& face, const std::vector<glm::vec3>& vertices);
	static std::vector<glm::vec3> FaceToNormals(const Face& face, const std::vector<glm::vec3>& normals);

	static glm::vec4 GenerateRandomColor();

private:
	static std::string GetFileName(const std::string& filePath);
};
