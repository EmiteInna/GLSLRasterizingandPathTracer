#include "RayTracing.h"
RayTracing::RayTracing(BVH* ibvh, Shader* shader1, Shader* shader2, Shader* shader3, Camera* cam) {
	bvh = ibvh;
	passpt = new Pass(shader1,bvh);
	pass1 = new Pass(shader2,bvh);
	pass2 = new Pass(shader3,bvh);
	w = 512;
	h = 512;
	lastframe = 0;
}
RayTracing::RayTracing() {
	bvh = NULL;
	passpt = NULL;
	pass1 = NULL;
	pass2 = NULL;
	w = 512;
	h = 512;
	lastframe = 0;
}
GLuint RayTracing::GetTexture(int w, int h) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return tex;
}
void RayTracing::RayTracingPrepare() {
	passpt->colorAttacments.push_back(GetTexture(w, h));
	passpt->colorAttacments.push_back(GetTexture(w, h));
	passpt->colorAttacments.push_back(GetTexture(w, h));
	glBindFragDataLocation(passpt->shaderID, 0, "outdata");
	passpt->bindData(true,false);

	lastframe = GetTexture(w, h);
	pass1->colorAttacments.push_back(lastframe);
	glBindFragDataLocation(pass1->shaderID, 0, "outdata");
	pass1->bindData(false, false);

	pass2->bindData(false, true);
}
void RayTracing::RayTracingRender(Camera* cam) {
	bvh->Render(cam);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, lastframe);
	glUniform1i(glGetUniformLocation(passpt->shaderID, "lastframe"), 15);
	passpt->draw(true,{});
	pass1->draw(false,passpt->colorAttacments);
	pass2->draw(false, pass1->colorAttacments);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}