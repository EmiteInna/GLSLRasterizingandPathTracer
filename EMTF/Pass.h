#ifndef EMITEINNA_PASS
#define EMITEINNA_PASS

#include "core.h"
#include"BVH.h"
class Pass {
public:
	BVH *bvh;
	GLuint FBO = 0;
	GLuint VBO;
	GLuint VAO;
	std::vector<GLuint> colorAttacments;
	GLuint shaderID;
	Pass(Shader* shader,BVH* bvh);
	void bindData(bool tracerPass, bool finalPass);
	void draw( bool tracer,std::vector<GLuint> texPassArray);
};

#endif // !EMITEINNA_PASS
