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
	
	if (model->showWire) {
		// Drag our model's faces (triangles) in line mode (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(model->GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model->GetModelVertices().size());
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
			lightColors[i] = light->color;
			lightLocations[i] = glm::vec4(light->location, 1);
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

//void Renderer::DrawSquare(vec3 vertices[4], vec3 color) {
//	vec3 a = vertices[0],
//		b = vertices[1],
//		c = vertices[2],
//		d = vertices[3];
//	DrawLine(a, b, color);
//	DrawLine(a, d, color);
//	DrawLine(b, c, color);
//	DrawLine(c, d, color);
//}
//
//void Renderer::DrawBoundingBox(glm::mat4 matrix, glm::vec3 min, glm::vec3 max)
//{
//	/*
//	 * points are represented by 3 sets of options:
//	 * 1.   front | back
//	 * 2.   top   | bottom
//	 * 3.   left  | right
//	 * 
//	 */
//	vec3
//		ftl = centerPoint(matrix * glm::vec4(min.x, max.y, min.z, 1)),
//		ftr = centerPoint(matrix * glm::vec4(max.x, max.y, min.z, 1)),
//		fbl = centerPoint(matrix * glm::vec4(min.x, min.y, min.z, 1)),
//		fbr = centerPoint(matrix * glm::vec4(max.x, min.y, min.z, 1)),
//		btl = centerPoint(matrix * glm::vec4(min.x, max.y, max.z, 1)),
//		btr = centerPoint(matrix * glm::vec4(max.x, max.y, max.z, 1)),
//		bbl = centerPoint(matrix * glm::vec4(min.x, min.y, max.z, 1)),
//		bbr = centerPoint(matrix * glm::vec4(max.x, min.y, max.z, 1));
//	vec3 color = vec3(0, 0.5, 0.5);
//
//	vec3 upperSquare[4] = { ftl, ftr, btr, btl };
//	vec3 lowerSquare[4] = { fbl, fbr, bbr, bbl };
//	vec3 rightSquare[4] = { ftr, btr, bbr, fbr };
//	vec3 leftSquare[4]  = { ftl, btl, bbl, fbl };
//
//	DrawSquare(lowerSquare, color);
//	DrawSquare(upperSquare, color);
//	DrawSquare(rightSquare, color);
//	DrawSquare(leftSquare, color);
//}
//glm::vec3 Renderer::CalcIllumination(Light& light, glm::vec3 p, glm::vec3 normal, glm::vec3 eye)
//{
//	glm::vec3 l = centerPoint(light.location),
//		lightVector = light.isPoint ? glm::normalize(p - l) : glm::normalize(-l),
//		mirror = 2.0f * (glm::dot(normal, lightVector)) - lightVector,
//		lookDirection = glm::normalize(p - eye);
//	float brightness = glm::max(glm::dot(glm::vec3(normal.x, normal.y, -normal.z), lightVector), 0.0f);
//	float cosPhi = glm::dot(lookDirection, mirror);
//	glm::vec3 illumination;
//
//	light.ambient = light.Ka * light.color;
//	light.diffuse = light.Kd * brightness * light.color;
//	light.specular = light.Ks * pow(cosPhi, light.shineOn) * light.color;
//
//	illumination = light.ambient + light.diffuse + light.specular;
//
//	if (fogActivated)
//		illumination = applyFog(illumination, p.z);
//
//	return illumination;
//}
//void Renderer::DrawTriangle(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, MeshModel* model, const Scene& scene, bool isLight) {
//	glm::vec3 p1 = vertices[0],
//		p2 = vertices[1],
//		p3 = vertices[2];
//	int minX = glm::min(p1.x, glm::min(p2.x, p3.x)),
//		minY = glm::min(p1.y, glm::min(p2.y, p3.y)),
//		maxX = glm::max(p1.x, glm::max(p2.x, p3.x)),
//		maxY = glm::max(p1.y, glm::max(p2.y, p3.y));
//	std::vector<Light*> lights = scene.GetLights();
//	glm::vec3 eye = scene.GetActiveCamera().location;
//	//eye = Utils::Mult(Utils::GetRotationMatrix(scene.GetActiveCamera().rotation), eye);
//	glm::vec3 faceNormal = Utils::CalcFaceNormal(vertices);
//	glm::vec3 c1 = model->color,
//		c2 = model->color2;
//	glm::vec3 p1Color = vec3(0), p2Color = vec3(0), p3Color = vec3(0);
//
//	for (int i = 0; i < lights.size(); i++) {
//		Light* light = lights[i];
//		p1Color += CalcIllumination(*light, p1, normals[0], eye) * c1;
//		p2Color += CalcIllumination(*light, p2, normals[1], eye) * c1;
//		p3Color += CalcIllumination(*light, p3, normals[2], eye) * c1;
//	}
//
//	for (int x = minX; x <= maxX; x++) {
//		for (int y = minY; y <= maxY; y++) {
//			glm::vec3 pxy(x, y, 0);
//			glm::vec2 bar = Utils::GetBarycentricCoords(pxy, vertices);
//			if (Utils::InTriangle(bar)) {
//				float lambda1 = 1 - bar.x - bar.y;
//				float lambda2 = bar.x;
//				float lambda3 = bar.y;
//				float z = 1 / (lambda1 * (1 / p1.z) + lambda2 * (1 / p2.z) + lambda3 * (1 / p3.z));
//				pxy.z = z;
//				glm::vec3 gouraudColor = (lambda1 * p1Color + lambda2 * p2Color + lambda3 * p3Color);
//				glm::vec3 normalush = -glm::normalize(lambda1 * normals[0] + lambda2 * normals[1] + lambda3 * normals[2]);
//				glm::vec3 finalColor = (scene.shadingType == GOURAUD) ? gouraudColor : c1;
//				glm::vec3 finalNormal = (scene.shadingType == FLAT) ? faceNormal : normalush;
//
//				glm::vec3 illumination(0);
//				for (int i = 0; i < lights.size(); i++) {
//					Light* light = lights[i];
//					illumination += CalcIllumination(*light, pxy, finalNormal, eye);
//				}
//
//				if (!model->uniformMaterial)
//					finalColor = Utils::GetMarbleColor(lambda1 + lambda3, c1, c2);
//
//				if (isLight)
//					illumination = glm::vec3(1);
//
//				float r = glm::min(finalColor.r * illumination.r, 1.0f);
//				float g = glm::min(finalColor.g * illumination.g, 1.0f);
//				float b = glm::min(finalColor.b * illumination.b, 1.0f);
//
//				putPixel(x, y, z, glm::vec3(r,g,b));
//			}
//		}
//	}
//}


void Renderer::LoadShaders()
{
	colorShader.loadShaders("vshader.glsl", "fshader.glsl");
}