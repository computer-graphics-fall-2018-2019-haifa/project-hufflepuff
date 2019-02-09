#pragma once
#include <glm/glm.hpp>
#include "MeshModel.h"

class Light : public MeshModel {
private:
public:
	int isPoint;
	Light();
};