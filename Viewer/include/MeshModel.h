#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Face.h"
#include "Texture2D.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

class MeshModel {
private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
	std::vector<Vertex> modelVertices;
	glm::mat4x4 worldTransform;
	std::string modelName;
	Texture2D texture;

public:
	bool showVertexNormals;
	bool showFacesNormals;
	bool showBoundingBox;
	bool fill;
	bool showWire;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 translation;
	glm::vec3 location;
	glm::vec4 color;
	glm::vec4 mins;
	glm::vec4 maxs;
	float Ka;
	float Kd;
	float Ks;
	int alpha;
	bool useTexture;

	GLuint vao; // vertex array object
	GLuint vbo; // vertex buffers object

	MeshModel() {};
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, std::vector<glm::vec2> textureCoords, const std::string& modelName = "");
	MeshModel(const MeshModel& other);
	virtual ~MeshModel();

	void InitOpenGL();

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

	GLuint GetVAO() const;
	const std::vector<Vertex>& GetModelVertices();

	void LoadTexture(const char * path);
	void BindTexture();
	void UnbindTexture();
};
