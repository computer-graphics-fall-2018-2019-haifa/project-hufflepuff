#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Camera.h"

/*
 * Scene class.
 * This class holds all the scene information (models, cameras, lights, etc..)
 */
class Scene {
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<Camera*> cameras;
	glm::mat4 worldTransformation;

public:
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 translation;
	int activeCameraIndex;
	int activeModelIndex;

	Scene();
	~Scene();

	void AddModel(const std::shared_ptr<MeshModel>& model);
	const int GetModelCount() const;

	void AddCamera(Camera* camera);
	const int GetCameraCount() const;

	void SetActiveCameraIndex(int index);
	const int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;

	// Add more methods as needed...
	std::vector<std::shared_ptr<MeshModel>> GetModels() const;
	std::vector<Camera*> GetCameras() const;
	const std::shared_ptr<MeshModel>& Scene::GetModel(int index) const;

	const Camera& GetActiveCamera() const;

	void SetWorldTransformation();
	glm::mat4 GetWorldTransformation() const;
};