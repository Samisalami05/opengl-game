#include "asset_manager.h"
#include "core/shader.h"
#include "core/texture.h"
#include "ecs.h"
#include "logger.h"
#include "resourcemanager.h"
#include "util/hash.h"
#include "util/hashmap.h"
#include "util/slotmap.h"
#include <stdint.h>
#include <string.h>

static AssetManager* assetman = NULL;

void asset_manager_init(AssetManager* am) {
	for (int i = 0; i < ASSET_LAST; i++) {
		slotmap_init(&am->assets[i], asset_type_size(i));
	}
	hashmap_init(&am->handles, ASSET_PATH_MAX, sizeof(AssetHandle), str_hash);

	assetman = am;
}

void asset_manager_deinit(AssetManager* am) {
	for (int i = 0; i < ASSET_LAST; i++) {
		slotmap_deinit(&am->assets[i]);
	}
	hashmap_deinit(&am->handles);

	assetman = NULL;
}

uint16_t asset_handle_type(AssetHandle handle) {
	return (handle & ASSET_HANDLE_TYPE_MASK) >> 48;
}

uint16_t asset_handle_gen(AssetHandle handle) {
	return (handle & ASSET_HANDLE_GEN_MASK) >> 32;
}

uint32_t asset_handle_id(AssetHandle handle) {
	return handle & ASSET_HANDLE_ID_MASK;
}

size_t asset_type_size(AssetType type) {
	switch (type) {
		case ASSET_TEXTURE: return sizeof(texture);
		case ASSET_SHADER: return sizeof(shader);
		case ASSET_LAST: return 0;
	}
}

static bool validate_handle(AssetHandle handle) {
	// If generation is lower than the stored index -> INVALID
	uint16_t type = asset_handle_type(handle);
	if (type >= ASSET_LAST) goto INVALID_HANDLE;

	// TODO: validate generation
	
	uint32_t id = asset_handle_id(handle);
	if (id >= assetman->assets[type].allocated)
		goto INVALID_HANDLE;
	if (!slotmap_is_occupied(&assetman->assets[type], id))
		goto INVALID_HANDLE;

	return true;

INVALID_HANDLE:
	LOG(LOG_ERROR, "Invalid asset handle");
	return false;
}

static AssetHandle asset_handle(uint16_t type, uint16_t gen, uint32_t id) {
	return ((uint64_t)type << 48) | ((uint64_t)gen << 32) | id;
}

// Returns ASSET_HANDLE_INVALID on failure
AssetHandle aload_texture(const char* path) {
	AssetHandle* existing = hashmap_get(&assetman->handles, path);
	if (existing != NULL) return *existing;

	texture tex = {0};
	//if (!texture_init(&tex, path)) return ASSET_HANDLE_INVALID;
	uint64_t slot = slotmap_add(&assetman->assets[ASSET_TEXTURE], &tex);
	AssetHandle handle = asset_handle(ASSET_TEXTURE, 0, slot);

	if (!hashmap_put(&assetman->handles, path, &handle))
		return ASSET_HANDLE_INVALID;
	return handle;
}

AssetHandle aload_shader(const char* vert, const char* frag) {
	char key[ASSET_PATH_MAX] = {0};
	strncpy(key, vert, ASSET_PATH_MAX / 2);
	strncat(key, frag, ASSET_PATH_MAX / 2);

	AssetHandle* existing = hashmap_get(&assetman->handles, key);
	if (existing != NULL) return *existing;

	shader shader = {0};
	//if (!shader_init(&shader, path)) return ASSET_HANDLE_INVALID;
	uint64_t slot = slotmap_add(&assetman->assets[ASSET_SHADER], &shader);
	AssetHandle handle = asset_handle(ASSET_SHADER, 0, slot);

	if (!hashmap_put(&assetman->handles, key, &handle))
		return ASSET_HANDLE_INVALID;
	return handle;
}

const void* asset_get(AssetHandle handle) {
	if (!validate_handle(handle)) return NULL;

	uint16_t type = asset_handle_type(handle);
	uint32_t id = asset_handle_id(handle);

	return slotmap_get(&assetman->assets[type], id);
}
