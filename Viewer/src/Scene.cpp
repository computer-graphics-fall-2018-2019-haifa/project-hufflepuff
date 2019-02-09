#include "Scene.h"
#include "MeshModel.h"
#include "Utils.h"
#include <string>
#include <iostream>

Scene::Scene() :
	activeCameraIndex(0),
	activeModelIndex(0),
	worldTransformation(glm::mat4(1)),
	scale(glm::vec3(1)),
	rotation(glm::vec3(0)),
	translation(glm::vec3(0)),
	shadingType(2),
	fogActivated(false)
{

}

Scene::~Scene()
{
	for (Camera* cam : cameras)
		delete cam;
	for (Light* light : lights)
		delete light;
}

void Scene::AddModel(const std::shared_ptr<MeshModel>& model)
{
	activeModelIndex = GetModelCount();
	models.push_back(model);
}

const int Scene::GetModelCount() const
{
	return models.size();
}

void Scene::AddLight(Light* light)
{
	activeLightIndex = GetLightCount();
	lights.push_back(light);
	light->SetModelName("lightSource" + std::to_string(activeLightIndex));
	light->scale = glm::vec3(0.5f);

	/*float theta = glm::angle(camera->at, camera->eye);
	camera->rotation = glm::vec3(theta);*/
}

const int Scene::GetLightCount() const
{
	return lights.size();
}

void Scene::AddCamera(Camera* camera)
{
	activeCameraIndex = GetCameraCount();
	cameras.push_back(camera);
	camera->SetModelName("camera" + std::to_string(activeCameraIndex));

	//float theta = glm::angle(camera->at, camera->eye);
	//camera->rotation = glm::vec3(0);
}

const int Scene::GetCameraCount() const
{
	return cameras.size();
}

void Scene::SetActiveCameraIndex(int index)
{
	if (index >= 0 && index < cameras.size())
	{
		activeCameraIndex = index;
	}
}

const int Scene::GetActiveCameraIndex() const
{
	return activeCameraIndex;
}

const int Scene::GetActiveLightIndex() const
{
	return activeLightIndex;
}

void Scene::SetActiveModelIndex(int index)
{
	if (index >= 0 && index < models.size())
	{
		activeModelIndex = index;
	}
}

const int Scene::GetActiveModelIndex() const
{
	return activeModelIndex;
}

std::vector<std::shared_ptr<MeshModel>> Scene::GetModels() const {
	return models;
}

std::vector<Camera*> Scene::GetCameras() const
{
	return cameras;
}

std::vector<Light*> Scene::GetLights() const
{
	return lights;
}

const MeshModel& Scene::GetModel(int index) const
{
	return *(models.at(index));
}

const Camera & Scene::GetCamera(int index) const
{
	return *cameras.at(index);
}

const Light & Scene::GetLight(int index) const
{
	return *lights.at(index);
}

const MeshModel& Scene::GetActiveModel() const
{
	return GetModel(activeModelIndex);
}

Camera & Scene::GetActiveCamera() const
{
	int i = GetActiveCameraIndex();
	return *cameras.at(i);
}

void Scene::SetWorldTransformation()
{
	this->worldTransformation = Utils::GetTransformationMatrix(scale, rotation, translation);
}

glm::mat4 Scene::GetWorldTransformation() const
{
	return this->worldTransformation;
}
