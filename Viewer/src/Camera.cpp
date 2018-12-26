#define _USE_MATH_DEFINES

#include "Camera.h"
#include "Utils.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define PI 3.14159265358979323846 / 180

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) :
	zoom(1.0), MeshModel(Utils::LoadMeshModel("..\\Data\\camera.obj")), projectionTransformation(glm::mat4(1)),
	eye(eye), at(at), up(up), isOrth(1), fovy(45), height(2.5f), aspectRatio(1), n(75), f(100),
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
	glm::mat4 translationMat = Utils::GetTranslationMatrix(translation);
	glm::mat4 rotationMat = Utils::GetRotationMatrix(rotation);
	glm::vec4 eye4 = Utils::Vec4FromVec3(eye);
	glm::vec4 at4 = Utils::Vec4FromVec3(at);
	glm::vec4 up4 = Utils::Vec4FromVec3(up);
	eye4 = rotationMat * eye4;
	at4 = translationMat /** rotationMatrix*/ * at4;
	//up4 = rotationMatrix * up4;
	location = Utils::Vec3FromVec4(eye4);

	glm::vec4 zAxis4 = glm::normalize(eye4 - at4);
	glm::vec3 zAxis3 = Utils::Vec3FromVec4(zAxis4);
	glm::vec3 xAxis3 = glm::normalize(glm::cross(Utils::Vec3FromVec4(up4), zAxis3));
	glm::vec3 yAxis3 = glm::normalize(glm::cross(zAxis3, xAxis3));

	glm::vec4 xAxis4 = Utils::Vec4FromVec3(xAxis3, 0);
	glm::vec4 yAxis4 = Utils::Vec4FromVec3(yAxis3, 0);
	glm::vec4 t4 = glm::vec4(0.0, 0.0, 0.0, 1.0);
	zAxis4 = glm::vec4(zAxis3, 0);

	glm::mat4 c(
		xAxis4,
		yAxis4,
		zAxis4,
		t4
	);
	c = glm::transpose(c);
	glm::mat4 identity(1);
	identity[3] -= eye4 - glm::vec4(0, 0, 0, 1);

	this->viewTransformation = c * identity * Utils::GetScaleMatrix(glm::vec3(zoom));
}

void Camera::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	MeshModel::SetWorldTransformation(worldTransform);
}

void Camera::SetWorldTransformation() {
	MeshModel::SetWorldTransformation();
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

	this->projectionTransformation = glm::mat4(
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (_near - _far), 0,
		-(r + l) / (r - l), -(t + b) / (t - b), -(_far + _near) / (_far - _near), 1
	);
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
	//float nearHeight = 2 * _near * tan(0.5 * _fovy * PI);
	float nearWidth = _aspectRatio * nearHeight;
	if (isAspect) {
		t = 0.5 * nearHeight;
		b = -0.5 * nearHeight;
		l = -0.5 * nearWidth;
		r = 0.5 * nearWidth;
	}

	this->projectionTransformation = glm::inverse(glm::mat4(
		(2 * _near) / (r - l), 0, (r + l) / (r - l), 0,
		0, (2 * _near) / (t - b), (t + b) / (t - b), 0,
		0, 0, -(_far + _near) / (_far - _near), -(2 * _far * _near) / (_far - _near),
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

glm::mat4 Camera::GetProjTransformation()
{
	return this->projectionTransformation;
}
