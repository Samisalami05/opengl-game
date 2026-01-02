#ifndef CUBEMAP_H
#define CUBEMAP_H

typedef struct cubemap {
	unsigned int id;
	// TODO: maybe add width and height for each face
} cubemap;

void cubemap_init(cubemap* cm, const char** faces);
void cubemap_deinit(cubemap* cm);
void cubemap_generate_mipmap(cubemap* cm);
void cubemap_use(cubemap* cm, unsigned int unit_index);

#endif
