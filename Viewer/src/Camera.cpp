#define _USE_MATH_DEFINES

#include "Camera.h"
#include "Utils.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define PI 3.14159265358979323846 / 180

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& m) :
	zoom(1.0), MeshModel(m), projectionTransformation(glm::mat4(1))
{
	SetCameraLookAt(eye, at, up);
	SetPerspectiveProjection(30, 1, 100, 1000);
	SetOrthographicProjection(1, 1, 10, 150);
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	glm::vec4 eye4 = Utils::Vec4FromVec3(eye);
	glm::vec4 at4 = Utils::Vec4FromVec3(at);
	glm::vec4 up4 = Utils::Vec4FromVec3(up);

	glm::vec4 zAxis4 = glm::normalize(eye4 - at4);
	glm::vec3 zAxis3 = glm::vec3(zAxis4.x, zAxis4.y, zAxis4.z);
	glm::vec3 xAxis3 = glm::normalize(glm::cross(up, zAxis3));
	glm::vec3 yAxis3 = glm::normalize(glm::cross(zAxis3, xAxis3));

	glm::vec4 xAxis4 = Utils::Vec4FromVec3(xAxis3, 0);
	glm::vec4 yAxis4 = Utils::Vec4FromVec3(yAxis3, 0);
	glm::vec4 t4 = glm::vec4(0.0, 0.0, 0.0, 1.0);

	glm::mat4 c(
		xAxis4,
		yAxis4,
		zAxis4,
		t4
	);
	c = glm::transpose(c);
	glm::mat4 identity(1);
	identity[3] -= eye4 - glm::vec4(0, 0, 0, 1);

	this->viewTransformation = c * identity;
}

void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float near,
	const float far)
{
	float width = height * aspectRatio;
	float t = 0.5 * height;
	float b = -0.5 * height;
	float l = -0.5 * width;
	float r = 0.5 * width;

	this->orthographicTransformation = glm::mat4(
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (near - far), 0,
		-(r + l) / (r - l), -(t + b) / (t - b), -(far + near) / (far - near), 1
	);
}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float near,
	const float far)
{
	float nearHeight = 2 * near * tan(0.5 * fovy * PI);
	float nearWidth = aspectRatio * nearHeight;
	float t = 0.5 * nearHeight;
	float b = -0.5 * nearHeight;
	float l = -0.5 * nearWidth;
	float r = 0.5 * nearWidth;

	this->projectionTransformation = glm::inverse(glm::mat4(
		(2 * near) / (r - l), 0, (r + l) / (r - l), 0,
		0, (2 * near) / (t - b), (t + b) / (t - b), 0,
		0, 0, -(far + near) / (far - near), -(2 * far * near) / (far - near),
		0, 0, -1, 0
	));
	this->projectionTransformation = glm::transpose(this->projectionTransformation);
}

void Camera::SetZoom(const float zoom)
{

}

glm::mat4 Camera::GetViewTransformation()
{
	return this->viewTransformation;
}

glm::mat4 Camera::GetOrthTransformation()
{
	return orthographicTransformation;
}

glm::mat4 Camera::GetPersTransformation()
{
	return projectionTransformation;
}
