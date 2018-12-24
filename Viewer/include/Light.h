#pragma once
#include <glm/glm.hpp>
#include "MeshModel.h"

class Light : public MeshModel {
private:
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int isPoint;
	int shineOn;
	float Ka;
	float Kd;
	float Ks;
	Light();
};