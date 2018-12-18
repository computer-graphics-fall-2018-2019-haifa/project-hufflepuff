#include "Light.h"
#include "Utils.h"
#include <math.h>

#define PI 3.14159265

Light::Light() :
	MeshModel(Utils::LoadMeshModel("..\\Data\\demo.obj")),
	type(0), reflection(0.25), Ka(1.0f), Kd(0.05), Ks(0.0f), shineOn(2.0f) {
	this->color = glm::vec4(1);
	this->translation = glm::vec3(0);
}

glm::vec3 Light::CalcIllumination(glm::vec3 p, glm::vec3 normal, glm::vec3 eye)
{
	normal = normal;
	glm::vec3 l = this->location, // glm::vec3(this->mins + this->maxs) / 2.0f,
		lightVector = - glm::normalize( l - p ),
		mirror = 2.0f * (glm::dot(normal, lightVector)) - lightVector,
		lookDirection = - glm::normalize(p - eye);
	float cosPhi = glm::dot(lookDirection, mirror);
	float brightness = glm::max(glm::dot(normal, lightVector), 0.0f);

	this->ambient =  Ka * color;
	this->diffuse =  Kd * brightness * color;
	this->specular = Ks * pow(cosPhi, shineOn) * color;

	return ambient + diffuse + specular;
}
