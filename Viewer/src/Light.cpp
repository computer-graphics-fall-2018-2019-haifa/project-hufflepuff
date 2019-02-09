#include "Light.h"
#include "Utils.h"
#include <math.h>

#define PI 3.14159265

Light::Light() :
	MeshModel(Utils::LoadMeshModel("..\\Data\\sphere.obj")),
	isPoint(true) {
	this->color = glm::vec4(1);
	this->translation = glm::vec3(10, 10, 0);
}
