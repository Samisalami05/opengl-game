#include "modelloader.h"
#include "core/mesh.h"
#include "math/vec3.h"
#include "util/arraylist.h"
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdio.h>


void load_model(model* m, const char* file) {
	const struct aiScene* scene = aiImportFile(
        file,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices
    );

	if (!scene) {
        printf("Assimp error: %s\n", aiGetErrorString());
        return;
    }

    printf("Meshes: %u\n", scene->mNumMeshes);
    printf("Materials: %u\n", scene->mNumMaterials);
	
	arraylist vertices, indices;
	arraylist_init(&vertices, sizeof(vertex));
	arraylist_init(&indices, sizeof(unsigned int));

	for (int i = 0; i < scene->mNumMeshes; i++) {
		struct aiMesh* aimesh = scene->mMeshes[i];
		mesh m;
		
		for (int j = 0; j < aimesh->mNumVertices; j++) {
			


			vertex v = {
				.pos = (vec3){aimesh->mVertices[j].x, aimesh->mVertices[j].y, aimesh->mVertices[j].z},
				.normal = aimesh->mNormals->x
			arraylist_append(&vertices, aimesh->mVertices[j].)

    aiReleaseImport(scene);
}
