#include "PathtracerMaterial.h"

PathTracerMaterial::PathTracerMaterial(glm::vec3 iemissive, glm::vec3 ibaseColor,
	float isubsurface, float imetallic, float ispecular, float ispecularTint, 
	float iroughness, float ianisotropic, float isheen, float isheenTint, 
	float iclearcoat, float iclearcoatgloss, float iior, float itransmission)
{
	emissive = iemissive;
	baseColor = ibaseColor;
	subsurface = isubsurface;
	metallic = imetallic;
	specular = ispecular;
	specularTint = ispecularTint;
	roughness = iroughness;
	anisotropic = ianisotropic;
	sheen = isheen;
	sheenTint = isheenTint;
	clearcoat = iclearcoat;
	clearcoatGloss = iclearcoatgloss;
	IOR = iior;
	transmission = itransmission;
}
PathTracerMaterial::PathTracerMaterial(glm::vec3 iemissive, glm::vec3 ibaseColor) {
	emissive = iemissive;
	baseColor = ibaseColor;
	subsurface = 0;
	metallic = 0;
	specular = 0;
	specularTint = 0;
	roughness = 0;
	anisotropic = 0;
	sheen = 0;
	sheenTint = 0;
	clearcoat = 0;
	clearcoatGloss = 0;
	IOR = 0;
	transmission = 0;
}
PathTracerMaterial::PathTracerMaterial() {
	emissive = glm::vec3(0);
	baseColor = glm::vec3(1);
	subsurface = 0;
	metallic = 0;
	specular = 0;
	specularTint = 0;
	roughness = 0;
	anisotropic = 0;
	sheen = 0;
	sheenTint = 0;
	clearcoat = 0;
	clearcoatGloss = 0;
	IOR = 0;
	transmission = 0;
}