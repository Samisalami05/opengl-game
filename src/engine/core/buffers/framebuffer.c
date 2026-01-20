#include "framebuffer.h"
#include <glad/glad.h>
#include <stdio.h>

void framebuffer_init(framebuffer* fbo) {
	glGenFramebuffers(1, &fbo->handle);

	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

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
