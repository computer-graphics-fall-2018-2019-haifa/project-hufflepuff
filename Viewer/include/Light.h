#pragma once
#include <glm/glm.hpp>
#include "MeshModel.h"

class Light : public MeshModel {
private:
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int type;
	int reflection;
	float strength;
	float Ka;
	float Kd;
	float Ks;
	float shineOn;
	glm::vec3 location;
	Light();

	glm::vec3 CalcIllumination(glm::vec3 p, glm::vec3 normal, glm::vec3 eye);
};