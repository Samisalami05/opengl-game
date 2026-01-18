#include "modelloader.h"
#include "core/mesh.h"
#include "core/texture.h"
#include "material.h"
#include "math/vec3.h"
#include "resourcemanager.h"
#include <assimp/cimport.h>
#include <assimp/color4.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <assimp/vector3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


static void load_mesh(struct aiMesh* aimesh, mesh* m) {

	int vertex_count = aimesh->mNumVertices;
	int index_count = aimesh->mNumFaces * 3;

	vertex* vertices = malloc(sizeof(vertex) * vertex_count);
	uint32_t* indices = malloc(sizeof(uint32_t) * index_count);

	for (int i = 0; i < aimesh->mNumVertices; i++) {
		struct aiVector3D vert = aimesh->mVertices[i];
		struct aiVector3D normal = aimesh->mNormals[i];
		struct aiVector3D* tex_coord = &aimesh->mTextureCoords[0][i];

		vertices[i] = (vertex){
			.pos = (vec3){ vert.x, vert.y, vert.z },
			.normal = (vec3){ normal.x, normal.y, normal.z },
			.uv = tex_coord != NULL ? (vec2){ tex_coord->x, tex_coord->y } : (vec2){ 0 },
		};
	}

	for (int i = 0; i < aimesh->mNumFaces; i++) {
		memcpy(indices + i * 3, (uint32_t*)aimesh->mFaces[i].mIndices, sizeof(uint32_t) * 3);
	}

	mesh_init(m, vertices, vertex_count, indices, index_count);

	free(vertices);
	free(indices);
}

static void get_directory(const char* filepath, char* out) {
    strcpy(out, filepath);
    char* last = strrchr(out, '/');
    if (last) *(last + 1) = '\0';
}

static void load_material(struct aiMaterial* aimat, material* mat, const char* modelpath) {
    material_init(mat, MAT_COLOR_LIT); // Default material

    struct aiString path;
    if (aiGetMaterialTexture(aimat, AI_MATKEY_BASE_COLOR_TEXTURE, &path, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", modelpath, path.data);

        mat->type = MAT_TEXTURE_LIT;
        mat->albedo_tex = load_texture(full_path);
	}

    struct aiColor4D ambient;
    if (aiGetMaterialColor(aimat, AI_MATKEY_COLOR_AMBIENT, &ambient) == AI_SUCCESS) {
        mat->color = (vec3){ ambient.r, ambient.g, ambient.b };
    }
}

void load_model(model* m, const char* file) {
	const struct aiScene* scene = aiImportFile(
        file,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices |
		aiProcess_PreTransformVertices
    );

	if (!scene) {
        printf("Assimp error: %s\n", aiGetErrorString());
        return;
    }

	char dir[512];
	get_directory(file, dir);

	printf("Model: %s\n", file);
    printf("Meshes: %u\n", scene->mNumMeshes);
    printf("Materials: %u\n", scene->mNumMaterials);

	m->mesh_count = scene->mNumMeshes;
	m->meshes = malloc(sizeof(mesh) * scene->mNumMeshes);
	m->mesh_mat_indices = malloc(sizeof(uint32_t) * scene->mNumMeshes);
	
	m->material_count = scene->mNumMaterials;
	m->materials = malloc(sizeof(material) * scene->mNumMaterials);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		struct aiMesh* aimesh = scene->mMeshes[i];
		load_mesh(aimesh, &m->meshes[i]);
		m->mesh_mat_indices[i] = aimesh->mMaterialIndex;
	}

	for (int i = 0; i < scene->mNumMaterials; i++) {
		struct aiMaterial* aimat = scene->mMaterials[i];
		load_material(aimat, &m->materials[i], dir);
	}

    aiReleaseImport(scene);
}

void model_deinit(model* m) {
	for (int i = 0; i < m->mesh_count; i++) {
		mesh_deinit(&m->meshes[i]);
	}

	for (int i = 0; i < m->material_count; i++) {
		material_deinit(&m->materials[i]);
	}

	free(m->meshes);
	free(m->materials);
}
