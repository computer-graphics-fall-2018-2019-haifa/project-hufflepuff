#define _USE_MATH_DEFINES

#include "Camera.h"
#include "Utils.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define PI 3.14159265358979323846 / 180

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) :
	zoom(1.0), MeshModel(Utils::LoadMeshModel("..\\Data\\camera.obj")), projectionTransformation(glm::mat4(1)),
	eye(eye), at(at), up(up), isOrth(1), fovy(45), height(2.5f), aspectRatio(1), n(0.1), f(100),
	t(1.25f), b(-1.25f), l(-1.25f), r(1.25f), isAspect(true)
{
	SetCameraLookAt(this->eye, this->at, this->up);
	this->rotation = glm::vec3(0);
	/*SetPerspectiveProjection(30, 1, 100, 1000);
	SetOrthographicProjection(1, 1, 10, 150);*/
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt() {
	this->SetCameraLookAt(eye, at, up);
}

void Camera::SetOrthographicProjection()
{
	this->SetOrthographicProjection(height, aspectRatio, n, f);
}

void Camera::SetCameraLookAt(glm::vec3& eye, glm::vec3& at, glm::vec3& up)
{
	this->SetWorldTransformation();
	this->viewTransformation = glm::lookAt(eye, at, up);

}

void Camera::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	MeshModel::SetWorldTransformation(worldTransform);
}

void Camera::SetWorldTransformation() {
	MeshModel::SetWorldTransformation();
	glm::mat4 invMat = glm::inverse(this->GetWorldTransformation());
	this->SetWorldTransformation(invMat);
}

void Camera::SetOrthographicProjection(
	const float _height,
	const float _aspectRatio,
	const float _near,
	const float _far)
{
	float width = _height * _aspectRatio;
	if (isAspect) {
		t = 0.5 * _height;
		b = -0.5 * _height;
		l = -0.5 * width;
		r = 0.5 * width;
	}
	this->projectionTransformation = glm::ortho(l, r, b, t, _near, _far);
}

void Camera::SetPerspectiveProjection() {
	this->SetPerspectiveProjection(fovy, aspectRatio, n, f);
}

void Camera::SetPerspectiveProjection(
	const float _fovy,
	const float _aspectRatio,
	const float _near,
	const float _far)
{
	float nearHeight = (_far - _near) * tan(_fovy * PI);
	float nearWidth = _aspectRatio * nearHeight;
	fovy = glm::min(_fovy, glm::degrees(glm::pi<float>()));

	if (isAspect) {
		t = 0.5 * nearHeight;
		b = -0.5 * nearHeight;
		l = -0.5 * nearWidth;
		r = 0.5 * nearWidth;
	}
	
	this->projectionTransformation = glm::perspective(fovy, _aspectRatio, _near, _far);
}

void Camera::SetZoom(const float zoom)
{
	if (this->isOrth) {
		this->SetOrthographicProjection(fovy * zoom, aspectRatio, n, f);
	}
	else {
		float val = glm::min(fovy * zoom, glm::pi<float>());
		this->SetPerspectiveProjection(val, aspectRatio, n, f);
	}
}

glm::mat4 Camera::GetViewTransformation() const {
	return this->viewTransformation;
}

glm::mat4 Camera::GetProjTransformation() const
{
	return this->projectionTransformation;
}
