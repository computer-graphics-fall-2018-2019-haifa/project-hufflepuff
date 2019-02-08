#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"

/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 *
 * Implementation suggestion:
 * --------------------------
 * Make the Camera class be a subclass of MeshModel, so you can easily and elegantly render 
 * the cameras you have added to the scene.
 */
class Camera: public MeshModel
{
private:
	glm::mat4x4 viewTransformation;
	glm::mat4x4 projectionTransformation;

public:
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	int isOrth;
	float height;
	float fovy;
	float aspectRatio;
	float n;
	float f;
	float zoom;
	bool isAspect;
	float t; // top
	float b; // bottom
	float l; // left
	float r; // right

	Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);
	~Camera();

	void SetCameraLookAt(glm::vec3& eye, glm::vec3& at, glm::vec3& up);
	void SetWorldTransformation(const glm::mat4x4 & worldTransform);
	void SetWorldTransformation();
	void SetCameraLookAt();

	void SetOrthographicProjection();

	void SetOrthographicProjection(
		const float height,
		const float aspectRatio,
		const float near,
		const float far);

	void SetPerspectiveProjection();

	void SetPerspectiveProjection(
		const float fovy,
		const float aspect,
		const float near,
		const float far);

	void SetZoom(const float zoom);

	// Add more methods/functionality as needed...
	glm::mat4 GetViewTransformation() const;
	glm::mat4 GetProjTransformation() const;
};
