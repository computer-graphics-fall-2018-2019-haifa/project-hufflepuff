#include "Scene.h"
#include "MeshModel.h"
#include "Utils.h"
#include <string>
#include <iostream>

Scene::Scene() :
	activeCameraIndex(0),
	activeModelIndex(0),
	worldTransformation(glm::mat4(1)),
	scale({ 1, 1, 1 }),
	rotation({ 0, 0, 0 }),
	translation({ 0, 0, 0 })
{

}

Scene::~Scene()
{
	for (Camera* cam : cameras)
		delete cam;
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

void Scene::AddCamera(Camera* camera)
{
	activeCameraIndex = GetCameraCount();
	cameras.push_back(camera);
	(*camera).SetModelName("camera" + std::to_string(activeCameraIndex));
	models.push_back(std::make_shared<MeshModel>(*camera));
}

const int Scene::GetCameraCount() const
{
	return cameras.size();
}

void Scene::SetActiveCameraIndex(int index)
{
	// implementation suggestion...
	if (index >= 0 && index < cameras.size())
	{
		activeCameraIndex = index;
	}
}

const int Scene::GetActiveCameraIndex() const
{
	return activeCameraIndex;
}

void Scene::SetActiveModelIndex(int index)
{
	// implementation suggestion...
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

const std::shared_ptr<MeshModel>& Scene::GetModel(int index) const
{
	return models.at(index);
}

const Camera & Scene::GetActiveCamera() const
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
	return worldTransformation;
}
