#ifndef EMITEINNA_RAYTRACING
#define EMITEINNA_RAYTRACING
#include "core.h"
#include "BVH.h"
#include "Pass.h"
class RayTracing{
public:
	int w, h;
	GLuint lastframe;
	BVH* bvh;
	Pass* passpt,* pass1,* pass2;
	RayTracing(BVH* ibvh, Shader* shader1, Shader* shader2, Shader* shader3,Camera* cam);
	RayTracing();
	GLuint GetTexture(int w, int h);
	void RayTracingPrepare();
	void RayTracingRender(Camera* cam);
};

#endif // !EMITEINNA_RAYTRACING
