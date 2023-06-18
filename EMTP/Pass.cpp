#include "Pass.h"
Pass::Pass(Shader* shader,BVH* ibvh) {
	shaderID = shader->ID;
	bvh = ibvh;
	colorAttacments.clear();
	VAO = 0;
	VBO = 0;
}

void Pass::bindData(bool tracerPass, bool finalPass) {
	if (!finalPass)glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glm::vec3 canvas[6] = {
		glm::vec3(1,-1,0),glm::vec3(-1,1,0),glm::vec3(-1,-1,0),
		glm::vec3(-1,1,0),glm::vec3(1,-1,0),glm::vec3(1,1,0)
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(canvas), &canvas[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	if (!finalPass) {
		std::vector<GLuint> attachments;
		for (int i = 0; i < colorAttacments.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, colorAttacments[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttacments[i], 0);
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(attachments.size(), &attachments[0]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pass::draw(bool tracer, std::vector<GLuint> texPassArray = {} ) {
	if(!tracer)glUseProgram(shaderID);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindVertexArray(VAO);
	for (int i = 0; i < texPassArray.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texPassArray[i]);
		std::string name = "texPass" + std::to_string(i);
		glUniform1i(glGetUniformLocation(shaderID, name.c_str()), i);
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}