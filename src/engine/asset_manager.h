#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "util/arraylist.h"
#include "util/hashmap.h"
#include "util/slotmap.h"
#include <stdbool.h>
#include <stdint.h>

#define ASSET_PATH_MAX 128

typedef enum {
	ASSET_TEXTURE,
	ASSET_SHADER,
	ASSET_LAST,
} AssetType;

// First 16 bits are for asset type, next 16 bits for generation 
// and the last 32 bits are for id.
// [ type | generation | index ]
//    16        16        32
typedef uint64_t AssetHandle;

#define ASSET_HANDLE_TYPE_MASK 0xFF000000
#define ASSET_HANDLE_GEN_MASK  0x00FF0000
#define ASSET_HANDLE_ID_MASK   0x0000FFFF

typedef struct {
	slotmap assets[ASSET_LAST];
	Hashmap handles;
} AssetManager;

// Attaches and initializes the given Asset manager.
void asset_manager_init(AssetManager* am);
void asset_manager_deinit(AssetManager* am);

uint16_t asset_handle_type(AssetHandle handle);
uint16_t asset_handle_gen(AssetHandle handle);
uint32_t asset_handle_id(AssetHandle handle);

size_t asset_type_size(AssetType type);

AssetHandle load_texture(const char* path);

void* asset_get(AssetHandle handle);

#endif
