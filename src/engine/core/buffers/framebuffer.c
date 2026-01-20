#include "framebuffer.h"
#include <glad/glad.h>
#include <stdio.h>

void framebuffer_init(framebuffer* fbo) {
	glGenFramebuffers(1, &fbo->handle);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Framebuffer is not complete\n");
		return;
	}
}

// Default usage is GL_FRAMEBUFFER
void framebuffer_use(framebuffer* fbo, GLenum usage) {
	glBindFramebuffer(usage, fbo->handle);
}

void framebuffer_unbind(framebuffer* fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_deinit(framebuffer* fbo) {
	glDeleteFramebuffers(1, &fbo->handle);
}
