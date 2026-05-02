#include "asset_manager.h"
#include "core/shader.h"
#include "core/texture.h"
#include "util/hash.h"
#include "util/hashmap.h"
#include "util/slotmap.h"
#include <stdint.h>

static AssetManager* assetman = NULL;

typedef struct {
	uint16_t type;
	uint32_t slot;
} AssetSlot;

size_t slot_hash(const void* key) {
	const AssetSlot* slot = key;
	return ((size_t)slot->type << 32) | slot->slot;
}

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
	AssetSlot slot = {
		.type = asset_handle_type(handle),
		.slot = asset_handle_id(handle),
	};

	return true;
}

AssetHandle load_texture(const char* path) {
	AssetHandle* existing = hashmap_get(&assetman->handles, path);
	if (existing != NULL) return *existing;

	texture t = {0};

}
void* asset_get(AssetHandle handle) {
	if (!validate_handle(handle)) return NULL;
}
