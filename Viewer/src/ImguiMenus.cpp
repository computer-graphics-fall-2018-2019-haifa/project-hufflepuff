#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Utils.h"
#include <cmath>
#include <math.h>
#include <memory>
#include <stdio.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <nfd.h>
#include <random>
#include <string>

bool showDemoWindow = false;
bool showAnotherWindow = false;
bool incremental = false;
bool lockScale = true;
bool lockRotation = true;
bool lockTranslation = true;

glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);

const glm::vec4& GetClearColor()
{
	return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer)
{
	std::vector<std::shared_ptr<MeshModel>> models = scene.GetModels();
	std::vector<Camera*> cameras = scene.GetCameras();
	std::vector<Light*> lights = scene.GetLights();

	int activeModelIndex = scene.GetActiveModelIndex();
	int activeCameraIndex = scene.GetActiveCameraIndex();
	int activeLightIndex = scene.GetActiveLightIndex();

	int modelsAmount = models.size();
	int camerasAmount = cameras.size();
	int lightsAmount = lights.size();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static int counter = 0;
		static int controlOverModel = 1;

		Camera* activeCamera = cameras.at(activeCameraIndex);
		Light* activeLight = lights.at(activeLightIndex);

		if (activeCamera->isOrth) {
			activeCamera->SetOrthographicProjection();
		}
		else {
			activeCamera->SetPerspectiveProjection();
		}

		ImGui::Begin("Mesh Model Viewer!");

		if (ImGui::CollapsingHeader("Models") && modelsAmount > 0) {
			std::shared_ptr<MeshModel> activeModel = models.at(activeModelIndex);
			char** modelNames = new char*[modelsAmount];
			for (int i = 0; i < modelsAmount; i++)
				modelNames[i] = const_cast<char*>((*models[i]).GetModelName().c_str());

			ImGui::Combo("Select Model", &scene.activeModelIndex, modelNames, modelsAmount);
			
			ImGui::Text("Active Model Preferences");
			ImGui::SameLine();
			if (ImGui::Button("Focus")) {
				glm::mat4 worldTransform = activeModel->GetWorldTransformation();
				activeCamera->at = activeModel->translation;
				activeCamera->SetCameraLookAt();
			}
			
			ImGui::Checkbox("Vertex Normals", &(activeModel->showVertexNormals));
			ImGui::Checkbox("Face Normals", &(activeModel->showFacesNormals));
			ImGui::Checkbox("Bouding Box", &(activeModel->showBoundingBox));

			ImGui::ColorEdit3("Color", (float*)&(activeModel->color));

			ImGui::Separator();
			ImGui::Text("Control over:");
			ImGui::RadioButton("Model", &(controlOverModel), 1);
			ImGui::RadioButton("World", &(controlOverModel), 0);
			ImGui::Separator();
			if (controlOverModel) {
				ImGui::Text("Scale");
				ImGui::Checkbox("Lock scale", &lockScale);
				if (lockScale) {
					ImGui::SliderFloat("Scale all", &(activeModel->scale.x), 1.0f, 100.0f);
					float x = activeModel->scale.x;
					activeModel->SetScale({ x, x, x });
				}
				else {
					ImGui::SliderFloat("Scale X", &(activeModel->scale.x), 0.0f, 1.0f);
					ImGui::SliderFloat("Scale Y", &(activeModel->scale.y), 0.0f, 1.0f);
					ImGui::SliderFloat("Scale Z", &(activeModel->scale.z), 0.0f, 1.0f);
				}

				ImGui::Separator();
				ImGui::Text("Rotate");
				ImGui::Checkbox("Lock rotation", &lockRotation);
				if (lockRotation) {
					ImGui::SliderFloat("Rotate all", &(activeModel->rotation.x), 0.0f, 360.0f);
					float x = activeModel->rotation.x;
					activeModel->SetRotation({ x, x, x });
				}
				else {
					ImGui::SliderFloat("Rotate X", &(activeModel->rotation.x), 0.0f, 360.0f);
					ImGui::SliderFloat("Rotate Y", &(activeModel->rotation.y), 0.0f, 360.0f);
					ImGui::SliderFloat("Rotate Z", &(activeModel->rotation.z), 0.0f, 360.0f);
				}

				ImGui::Separator();
				ImGui::Text("Translate");
				ImGui::Checkbox("Lock translation", &lockTranslation);
				if (lockTranslation) {
					ImGui::SliderFloat("Trans. all", &(activeModel->translation.x), -1.0f, 1.0f);
					float x = activeModel->translation.x;
					activeModel->SetTranslation({ x, x, x });
				}
				else {
					ImGui::SliderFloat("Around X", &(activeModel->translation.x), -1.0f, 1.0f);
					ImGui::SliderFloat("Around Y", &(activeModel->translation.y), -1.0f, 1.0f);
					ImGui::SliderFloat("Around Z", &(activeModel->translation.z), -1.0f, 1.0f);
				}
			}
			else {
				ImGui::Text("Scale");
				ImGui::Checkbox("Lock scale", &lockScale);
				if (lockScale) {
					ImGui::SliderFloat("Scale world", &(scene.scale.x), 1.0f, 100.0f);
					float x = scene.scale.x;
					scene.scale = { x, x, x };
				}
				else {
					ImGui::SliderFloat("Scale.w X", &(scene.scale.x), 1.0f, 200.0f);
					ImGui::SliderFloat("Scale.w Y", &(scene.scale.y), 1.0f, 200.0f);
					ImGui::SliderFloat("Scale.w Z", &(scene.scale.z), 1.0f, 200.0f);
				}

				ImGui::Separator();
				ImGui::Text("Rotate");
				ImGui::Checkbox("Lock rotation", &lockRotation);
				if (lockRotation) {
					ImGui::SliderFloat("Rotate world", &(scene.rotation.x), 0.0f, 360.0f);
					float x = scene.rotation.x;
					scene.rotation = { x, x, x };
				}
				else {
					ImGui::SliderFloat("Rotate.w X", &(scene.rotation.x), 0.0f, 360.0f);
					ImGui::SliderFloat("Rotate.w Y", &(scene.rotation.y), 0.0f, 360.0f);
					ImGui::SliderFloat("Rotate.w Z", &(scene.rotation.z), 0.0f, 360.0f);
				}
			}

			ImGui::Separator();
			ImGui::Text("Reflection:");
			ImGui::SliderFloat("Ambient", &(activeModel->Ka), 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse", &(activeModel->Kd), 0.0f, 1.0f);
			ImGui::SliderFloat("Specular", &(activeModel->Ks), 0.0f, 1.0f);
			ImGui::Text("Specular option:");
			ImGui::SliderInt("Alpha", &(activeModel->alpha), 1, 500);

			activeModel->SetWorldTransformation();
			scene.SetWorldTransformation();

			delete [] modelNames;
		}

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Cameras") && camerasAmount > 0) {
			static int controlCam = 0;

			if (ImGui::Button("Add Camera")) {
				glm::vec3 eye = activeCamera->eye,
					at = activeCamera->at,
					up = activeCamera->up;
				Camera *c = new Camera(at, eye, up);
				scene.AddCamera(c);
			}

			char** cameraNames = new char*[camerasAmount];
			for (int i = 0; i < camerasAmount; i++)
				cameraNames[i] = const_cast<char*>(cameras[i]->GetModelName().c_str());

			ImGui::Combo("Select camera", &scene.activeCameraIndex, cameraNames, camerasAmount);
			
			ImGui::Text("Active Camera Preferences:");
			ImGui::Separator();
			ImGui::RadioButton("Rotate", &controlCam, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Translate", &controlCam, 1);
			if (controlCam == 0) {
				ImGui::SliderFloat("Rotate.c X", &(activeCamera->rotation.x), 0.0f, 360.0f);
				ImGui::SliderFloat("Rotate.c Y", &(activeCamera->rotation.y), 0.0f, 360.0f);
				ImGui::SliderFloat("Rotate.c Z", &(activeCamera->rotation.z), 0.0f, 360.0f);
			}
			else {
				ImGui::SliderFloat("Trans.c X", &(activeCamera->eye.x), -20.0f, 20.0f);
				ImGui::SliderFloat("Trans.c Y", &(activeCamera->eye.y), -20.0f, 20.0f);
				ImGui::SliderFloat("Trans.c Z", &(activeCamera->eye.z), -20.0f, 20.0f);
			}

			ImGui::Separator();
			ImGui::SliderFloat("Zoom", &(activeCamera->zoom), 0.0f, 50.0f);
			ImGui::Separator();
			ImGui::RadioButton("Orthographic", &(activeCamera->isOrth), 1);
			ImGui::RadioButton("Perspective", &(activeCamera->isOrth), 0);
			ImGui::Separator();
			ImGui::SliderFloat("Near", &(activeCamera->n), 0.0f, 99.0f);
			ImGui::SliderFloat("Far", &(activeCamera->f), 100.0f, 1000.0f);

			activeCamera->translation = activeCamera->eye;

			delete[] cameraNames;
		}

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Lights") && lightsAmount > 0) {
			Light* activeLight = lights.at(activeLightIndex);
			if (lights.size() < 5) {
				if (ImGui::Button("Add Light")) {
					Light *l = new Light();
					scene.AddLight(l);
				}
			}

			char** lightNames = new char*[lightsAmount];
			for (int i = 0; i < lightsAmount; i++)
				lightNames[i] = const_cast<char*>(lights[i]->GetModelName().c_str());

			ImGui::Combo("Select light", &scene.activeLightIndex, lightNames, lightsAmount);

			ImGui::Text("Active Light Preferences:");
			ImGui::Separator();
			ImGui::ColorEdit3("Light color", (float*)&(activeLight->color));

			/*ImGui::Separator();
			ImGui::Text("Light type:");
			ImGui::RadioButton("Point", &(activeLight->isPoint), 1);
			ImGui::RadioButton("Parallel", &(activeLight->isPoint), 0);*/

			ImGui::Separator();
			//if (activeLight->isPoint) {
			ImGui::Text("Translate Light");
			ImGui::SliderFloat("Trans.l X", &(activeLight->translation.x), -400.0f, 400.0f);
			ImGui::SliderFloat("Trans.l Y", &(activeLight->translation.y), -400.0f, 400.0f);
			ImGui::SliderFloat("Trans.l Z", &(activeLight->translation.z), -400.0f, 400.0f);
			glm::vec3 point = activeLight->GetVertices()[0];
			activeLight->location = Utils::Mult(activeLight->GetWorldTransformation() * scene.GetWorldTransformation(), point);

			delete[] lightNames;
		}
		else {
			glm::vec3 point = activeLight->GetVertices()[0];
			activeLight->location = Utils::Mult(activeLight->GetWorldTransformation() * scene.GetWorldTransformation(), point);
		}
		
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Scene")) {
			if (ImGui::ColorEdit3("BG color", (float*)&clearColor)) {
				glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			}

			/*ImGui::Separator();
			ImGui::Text("Shading method:");
			ImGui::RadioButton("Gouraud", &(scene.shadingType), 1);
			ImGui::RadioButton("Phong", &(scene.shadingType), 2);*/
		}

		// update stuff.
		activeCamera->SetCameraLookAt();
		activeLight->SetWorldTransformation();
		ImGui::End();
	}

	// 3. Show another simple window.
	if (showAnotherWindow)
	{
		ImGui::Begin("Another Window", &showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
		{
			showAnotherWindow = false;
		}
		ImGui::End();
	}

	// 4. Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Model...", "CTRL+O"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						scene.AddModel(std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath)));
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}