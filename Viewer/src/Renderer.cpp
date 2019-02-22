#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include "Utils.h"
#include <iostream>
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include <math.h>
#include <algorithm>

#define FLAT 0
#define GOURAUD 1
#define PHONG 2

Renderer::Renderer() :
	fogActivated(false),
	fogColor(0.343f, 0.105f, 0.667f)
{

}

Renderer::~Renderer()
{

}

void Renderer::DrawModel(const Scene& scene, MeshModel* model) {
	glm::mat4 modelMat = scene.GetWorldTransformation() * model->GetWorldTransformation();

	// Set the uniform variables
	colorShader.setUniform("model", modelMat);
	colorShader.setUniform("material.color", model->color);
	colorShader.setUniform("material.Ka", model->Ka);
	colorShader.setUniform("material.Kd", model->Kd);
	colorShader.setUniform("material.Ks", model->Ks);
	colorShader.setUniform("material.alpha", model->alpha);
	colorShader.setUniform("useTexture", model->useTexture);

	if (model->fill) {
		// Set the model's texture as the active texture at slot #0
		model->BindTexture();

		// Drag our model's faces (triangles) in fill mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(model->GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model->GetModelVertices().size());
		glBindVertexArray(0);

		// Unset the model's texture as the active texture at slot #0
		model->UnbindTexture();
	}

	colorShader.setUniform("useTexture", false);
	
	if (model->showWire) {
		colorShader.setUniform("material.color", glm::vec4(0, 0, 1, 1));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(model->GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model->GetModelVertices().size());
		glBindVertexArray(0);
	}

	if (model->showBoundingBox) {
		colorShader.setUniform("material.color", glm::vec4(1, 0, 0, 1));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(model->boxVao);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model->GetBoundingBoxVertices().size());
		glBindVertexArray(0);
	}

	if (model->showVertexNormals) {
		colorShader.setUniform("material.color", glm::vec4(0, 1, 0, 1));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(model->normalVao);
		glDrawArrays(GL_LINES, 0, (GLsizei)model->GetVertexNormals().size());
		glBindVertexArray(0);
	}
}

void Renderer::Render(const Scene& scene)
{
	if (!scene.GetCameraCount()) {
		return;
	}

	std::vector<std::shared_ptr<MeshModel>> models = scene.GetModels();
	std::vector<Camera*> cameras = scene.GetCameras();
	std::vector<Light*> lights = scene.GetLights();

	Camera activeCamera = scene.GetActiveCamera();
	glm::mat4 viewMat = activeCamera.GetViewTransformation();
	glm::mat4 objMat = activeCamera.GetWorldTransformation();
	glm::mat4 projMat = activeCamera.GetProjTransformation();
	glm::vec4 lightColors[5] = { glm::vec4(0) };
	glm::vec4 lightLocations[5] = { glm::vec4(0) };

	for (int i = 0; i < 5; i++) {
		if (i < lights.size()) {
			Light* light = lights.at(i);
			glm::vec3 pos = light->location;
			lightColors[i] = light->color;
			lightLocations[i] = glm::vec4(pos.x, pos.y, -pos.z, 1);
		}
	}
	
	colorShader.use();

	// camera params
	colorShader.setUniform("view", viewMat * objMat);
	colorShader.setUniform("projection", projMat);
	colorShader.setUniform("lightColors", lightColors);
	colorShader.setUniform("lightLocations", lightLocations);
	
	// draw models
	for (std::shared_ptr<MeshModel> model : models) {
		model->SetWorldTransformation();
		DrawModel(scene, &(*model));
	}
	
	// draw cameras
	for (int i = 0; i < scene.GetCameraCount(); i++) {
		if (scene.GetActiveCameraIndex() == i)
			continue;
		MeshModel* model = cameras.at(i);
		model->scale = glm::vec3(0.1);
		model->SetWorldTransformation();
		DrawModel(scene, model);
	}

	// draw lights
	for (int i = 0; i < scene.GetLightCount(); i++) {
		MeshModel* model = lights.at(i);
		model->scale = glm::vec3(0.1);
		model->SetWorldTransformation();
		DrawModel(scene, &(*model));
	}
}

void Renderer::LoadShaders()
{
	colorShader.loadShaders("vshader.glsl", "fshader.glsl");
}