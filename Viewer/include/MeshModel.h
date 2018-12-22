#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Face.h"

/*
 * MeshModel class.
 * This class represents a mesh model (with faces and normals informations).
 * You can use Utils::LoadMeshModel to create instances of this class from .obj files.
 */
class MeshModel
{
private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4x4 worldTransform;
	std::string modelName;

public:
	bool showVertexNormals;
	bool showFacesNormals;
	bool showBoundingBox;
	bool uniformMaterial;
	glm::vec3 avg;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 translation;
	glm::vec3 location;
	glm::vec4 color;
	glm::vec4 color2;
	glm::vec4 mins;
	glm::vec4 maxs;

	MeshModel() {};
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName = "");
	MeshModel(const MeshModel& other);
	virtual ~MeshModel();

	void SetWorldTransformation();
	virtual void SetWorldTransformation(const glm::mat4x4& worldTransform);
	void SetWorldTransformation(glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate);
	const glm::mat4x4& GetWorldTransformation() const;

	const glm::vec4& GetColor() const;
	void SetColor(const glm::vec4& color);

	const std::string& GetModelName();
	void SetModelName(std::string name);

	// Add more methods/functionality as needed...
	const std::vector<glm::vec3> GetVertices() const;
	const std::vector<glm::vec3>& GetNormals() const;
	const std::vector<Face>& GetFaces() const;

	const glm::vec4 GetMin() const;
	const glm::vec4 GetMax() const;

	const glm::vec3 GetScale() const;
	const glm::vec3 GetRotation() const;
	const glm::vec3 GetTranslation() const;
	void SetScale(glm::vec3 _s);
	void SetRotation(glm::vec3 _r);
	void SetTranslation(glm::vec3 _t);
};
