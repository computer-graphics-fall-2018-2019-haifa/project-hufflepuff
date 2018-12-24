#include "Light.h"
#include "Utils.h"
#include <math.h>

#define PI 3.14159265

Light::Light() :
	MeshModel(Utils::LoadMeshModel("..\\Data\\sphere.obj")),
	isPoint(true), Ka(0.5f), Kd(0.7), Ks(0.2f), shineOn(3.0f) {
	this->color = glm::vec4(1);
	this->translation = glm::vec3(400, 400, 0);
}
