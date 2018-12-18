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

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
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

		/*if (ImGui::IsMouseDragging(0)) {
			static glm::vec3 lastPos = activeCamera->eye;
			ImVec2 v = ImGui::GetMouseDragDelta();
			lastPos -= glm::vec3(v.x, v.y, 0);
			activeCamera->eye = lastPos;
		}*/
		

		if (activeCamera->isOrth) {
			activeCamera->SetOrthographicProjection();
		}
		else {
			activeCamera->SetPerspectiveProjection();
		}

		ImGui::Begin("Mesh Model Viewer!");

		ImGui::ColorEdit3("BG color", (float*)&clearColor);

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
			ImGui::ColorEdit3("Color", (float*)&(activeModel->color));
			ImGui::Checkbox("Vertex Normals", &(activeModel->showVertexNormals));
			ImGui::Checkbox("Face Normals", &(activeModel->showFacesNormals));
			ImGui::Checkbox("Bouding Box", &(activeModel->showBoundingBox));
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
					ImGui::SliderFloat("Scale X", &(activeModel->scale.x), 1.0f, 200.0f);
					ImGui::SliderFloat("Scale Y", &(activeModel->scale.y), 1.0f, 200.0f);
					ImGui::SliderFloat("Scale Z", &(activeModel->scale.z), 1.0f, 200.0f);
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
					ImGui::SliderFloat("Trans. all", &(activeModel->translation.x), -100.0f, 100.0f);
					float x = activeModel->translation.x;
					activeModel->SetTranslation({ x, x, x });
				}
				else {
					ImGui::SliderFloat("Around X", &(activeModel->translation.x), -100.0f, 100.0f);
					ImGui::SliderFloat("Around Y", &(activeModel->translation.y), -100.0f, 100.0f);
					ImGui::SliderFloat("Around Z", &(activeModel->translation.z), -100.0f, 100.0f);
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

			activeModel->SetWorldTransformation();
			scene.SetWorldTransformation();

			delete [] modelNames;
		}

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Cameras") && camerasAmount > 0) {
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

			ImGui::Separator();
			ImGui::Text("Rotate");
			ImGui::SliderFloat("Rotate.c X", &(activeCamera->rotation.x), 0.0f, 360.0f);
			ImGui::SliderFloat("Rotate.c Y", &(activeCamera->rotation.y), 0.0f, 360.0f);
			ImGui::SliderFloat("Rotate.c Z", &(activeCamera->rotation.z), 0.0f, 360.0f);
			ImGui::Separator();
			ImGui::Text("Active Camera Preferences:");
			ImGui::SliderFloat("Eye X", &(activeCamera->eye.x), -1000.0f, 1000.0f);
			ImGui::SliderFloat("Eye Y", &(activeCamera->eye.y), -1000.0f, 1000.0f);
			ImGui::SliderFloat("Eye Z", &(activeCamera->eye.z), -1000.0f, 1000.0f);
			ImGui::Separator();
			ImGui::SliderFloat("At X", &(activeCamera->at.x), -100.0f, 100.0f);
			ImGui::SliderFloat("At Y", &(activeCamera->at.y), -100.0f, 100.0f);
			ImGui::SliderFloat("At Z", &(activeCamera->at.z), -100.0f, 100.0f);
			ImGui::Separator();
			ImGui::SliderFloat("Up X", &(activeCamera->up.x), -100.0f, 100.0f);
			ImGui::SliderFloat("Up Y", &(activeCamera->up.y), -100.0f, 100.0f);
			ImGui::SliderFloat("Up Z", &(activeCamera->up.z), -100.0f, 100.0f);

			ImGui::Separator();
			ImGui::SliderFloat("Zoom", &(activeCamera->zoom), 1.0f, 3.0f);
			ImGui::Separator();
			ImGui::RadioButton("Orthographic", &(activeCamera->isOrth), 1);
			ImGui::RadioButton("Perspective", &(activeCamera->isOrth), 0);

			if (activeCamera->isOrth) {
				ImGui::SliderFloat("Height", &(activeCamera->height), 1.0f, 100.0f);
			}
			else {
				ImGui::SliderFloat("Fovy", &(activeCamera->fovy), 0.0f, 180.0f);
			}

			ImGui::Checkbox("Aspect", &(activeCamera->isAspect));
			if (activeCamera->isAspect) {
				ImGui::SliderFloat("Aspect Ratio", &(activeCamera->aspectRatio), 0.1f, 2.0f);
			}
			else {
				ImGui::SliderFloat("Top", &(activeCamera->t), 0.1f, 5.0f);
				ImGui::SliderFloat("Bottom", &(activeCamera->b), -5.0f, -0.1f);
				ImGui::SliderFloat("Right", &(activeCamera->r), 0.1f, 5.0f);
				ImGui::SliderFloat("Left", &(activeCamera->l), -5.0f, -0.1f);
				activeCamera->aspectRatio = (activeCamera->t - activeCamera->b);
				activeCamera->aspectRatio /= (activeCamera->r - activeCamera->l);
			}
			ImGui::SliderFloat("Near", &(activeCamera->n), 10.0f, 100.0f);
			ImGui::SliderFloat("Far", &(activeCamera->f), 100.0f, 1000.0f);

			//scene.scale = glm::vec3(activeCamera->zoom);
			activeCamera->translation = activeCamera->eye;
			activeCamera->SetCameraLookAt();
			//activeCamera->SetWorldTransformation();
			//scene.SetWorldTransformation();

			delete[] cameraNames;
		}

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Lights") && lightsAmount > 0) {
			Light* activeLight = lights.at(activeLightIndex);
			if (ImGui::Button("Add Light")) {
				Light *l = new Light();
				scene.AddLight(l);
			}

			char** lightNames = new char*[lightsAmount];
			for (int i = 0; i < lightsAmount; i++)
				lightNames[i] = const_cast<char*>(lights[i]->GetModelName().c_str());

			ImGui::Combo("Select light", &scene.activeLightIndex, lightNames, lightsAmount);

			ImGui::Separator();
			ImGui::ColorEdit3("Light color", (float*)&(activeLight->color));

			ImGui::Separator();
			ImGui::Text("Translate Light");
			ImGui::SliderFloat("Trans.l X", &(activeLight->translation.x), -400.0f, 400.0f);
			ImGui::SliderFloat("Trans.l Y", &(activeLight->translation.y), -400.0f, 400.0f);
			ImGui::SliderFloat("Trans.l Z", &(activeLight->translation.z), -400.0f, 400.0f);

			ImGui::Separator();
			ImGui::Text("Reflection:");
			ImGui::SliderFloat("Ambient ref",  &(activeLight->Ka), 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse ref",  &(activeLight->Kd), 0.0f, 1.0f);
			ImGui::SliderFloat("Specular ref", &(activeLight->Ks), 0.0f, 1.0f);
			ImGui::SliderFloat("Shiny", &(activeLight->shineOn), 0.0f, 3.0f);

			ImGui::Separator();
			ImGui::Text("Shading method:");
			ImGui::RadioButton("Flat", &(activeLight->type), 0);
			ImGui::RadioButton("Gouraud", &(activeLight->type), 1);
			ImGui::RadioButton("Phong", &(activeLight->type), 2);

			/*ImGui::Separator();
			ImGui::Text("Light Type:");
			ImGui::RadioButton("Point",    &(activeLight->type), 0);
			ImGui::RadioButton("Parallel", &(activeLight->type), 1);*/

			activeLight->location = Utils::Mult(Utils::TransMatricesLight(scene, activeLightIndex), activeLight->translation);

			delete[] lightNames;
		}
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