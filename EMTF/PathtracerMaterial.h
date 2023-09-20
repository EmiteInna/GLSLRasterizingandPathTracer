#ifndef EMITEINNA_PATHTRACER_MATERIAL
#define EMITEINNA_PATHTRACER_MATERIAL

#include "core.h"
class PathTracerMaterial {
public:
	glm::vec3 emissive;
	glm::vec3 baseColor;
	float subsurface;
	float metallic;
	float specular;
	float specularTint;
	float roughness;
	float anisotropic;
	float sheen;
	float sheenTint;
	float clearcoat;
	float clearcoatGloss;
	float IOR;
	float transmission;
	PathTracerMaterial(glm::vec3 emissive,glm::vec3 baseColor);
	PathTracerMaterial(glm::vec3 emissive, glm::vec3 baseColor, float subsurface, float metallic, float specular, float specularTint, float roughness, float anisotropic, float sheen, float sheenTint, float clearcoat, float clearcoatgloss, float ior, float transmission);
	PathTracerMaterial();
};


#endif