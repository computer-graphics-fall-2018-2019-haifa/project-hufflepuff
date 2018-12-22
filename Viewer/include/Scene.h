#pragma once

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Camera.h"
#include "Light.h"

/*
 * Scene class.
 * This class holds all the scene information (models, cameras, lights, etc..)
 */
class Scene {
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
	glm::mat4 worldTransformation;

public:
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 translation;
	int activeCameraIndex;
	int activeModelIndex;
	int activeLightIndex;
	int shadingType;
	bool fogActivated;

	Scene();
	~Scene();

	void AddModel(const std::shared_ptr<MeshModel>& model);
	const int GetModelCount() const;

	void AddLight(Light * light);
	const int GetLightCount() const;

	void AddCamera(Camera* camera);
	const int GetCameraCount() const;

	void SetActiveCameraIndex(int index);
	const int GetActiveCameraIndex() const;

	const int GetActiveLightIndex() const;

	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;

	// Add more methods as needed...
	std::vector<std::shared_ptr<MeshModel>> GetModels() const;
	std::vector<Camera*> GetCameras() const;
	std::vector<Light*> GetLights() const;
	const MeshModel & GetModel(int index) const;
	const Camera & GetCamera(int index) const;
	const Light & GetLight(int index) const;
	const MeshModel& GetActiveModel() const;

	const Camera& GetActiveCamera() const;

	void SetWorldTransformation();
	glm::mat4 GetWorldTransformation() const;
};