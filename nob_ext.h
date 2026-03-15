/*
**      ███╗   ██╗ ██████╗ ██████╗         ███████╗██╗  ██╗████████╗██╗  ██╗
**      ████╗  ██║██╔═══██╗██╔══██╗        ██╔════╝╚██╗██╔╝╚══██╔══╝██║  ██║
**      ██╔██╗ ██║██║   ██║██████╔╝        █████╗   ╚███╔╝    ██║   ███████║
**      ██║╚██╗██║██║   ██║██╔══██╗        ██╔══╝   ██╔██╗    ██║   ██╔══██║
**      ██║ ╚████║╚██████╔╝██████╔╝███████╗███████╗██╔╝ ██╗   ██║██╗██║  ██║
**      ╚═╝  ╚═══╝ ╚═════╝ ╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝   ╚═╝╚═╝╚═╝  ╚═╝
**
**
**  This library is a extension to the [nob.h](https://github.com/tsoding/nob.h)
**    no build library written by tsoding. The library is meant to expand the 
**    behaviour of nob.h with both useful but also build system like features. 
** 
**  In order to use the library, you need a copy of nob.h and you have to define 
**                     NOB_IMPLEMENTATION before including.
**
**                                 Enjoy :)
*/                                                                    

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#define NOB_IMPLEMENTATION // TODO: REMOVE

#include "nob.h"

typedef struct {
	const char* path;
	const char* source;
	Nob_Cmd deps;
} Nob_Object_File;

typedef struct {
	Nob_Object_File* items;
	size_t count;
	size_t capacity;
} Nob_Object_Files;

typedef enum {
	NOB_BUILD_CLEAN,
	NOB_BUILD_INCREMENTAL,
} Nob_Build_Type;

typedef struct { // TODO maybe add flags here
	Nob_Build_Type build_type;
	const char* source_path;
	const char* dest_path;
	//const char* file_extenstion;
} Nob_Comp_Opts;

typedef struct {
	Nob_Comp_Opts opts;
	Nob_Cmd includes;
	Nob_Cmd links;
	Nob_Cmd flags;
} Nob_Comp_Args;


// --- Util ---

// Only waits for the necessary amount of processes before adding the new one.
// Can be more efficient that 'nob_procs_append_with_flush' with different process
// loads.
NOBDEF bool nob_procs_append(Nob_Procs *procs, Nob_Proc proc, size_t max_procs_count);

NOBDEF Nob_Cmd nob_file_deps(const char* path, Nob_Comp_Args args);
NOBDEF Nob_Object_File nob_construct_obj(const char* path, Nob_Comp_Args args);
NOBDEF void nob_objs_append(Nob_Object_Files* objs, Nob_Object_File obj);
NOBDEF bool nob_obj_should_build(Nob_Object_File obj);

// TODO: async is deprecated, use cmd opts
NOBDEF Nob_Proc nob_build_object_async(Nob_Object_File obj, Nob_Comp_Args args, bool* skipped);
NOBDEF Nob_Procs nob_build_objects_async(Nob_Object_Files objs, Nob_Comp_Args args);
NOBDEF bool nob_build_object(Nob_Object_File obj, Nob_Comp_Args args);
NOBDEF bool nob_build_objects(Nob_Object_Files objs, Nob_Comp_Args args);


NOBDEF bool nob_build_walk(Nob_Walk_Entry entry);

#ifdef NOB_IMPLEMENTATION

NOBDEF bool nob_procs_append(Nob_Procs *procs, Nob_Proc proc, size_t max_procs_count)
{
    nob_da_append(procs, proc);

	bool success = true;
    if (procs->count >= max_procs_count) {
		size_t overflow = procs->count - max_procs_count;

		for (size_t i = 0; i < overflow; i++) {
			success = nob_proc_wait(procs->items[i]) && success;
		}

		memcpy(procs->items, procs->items + overflow, sizeof(Nob_Proc) * max_procs_count);
		procs->count -= overflow;
    }

    return success;
}


// TODO: do async
NOBDEF Nob_Cmd nob_file_deps(const char* path, Nob_Comp_Args args) {
	Nob_Cmd cmd = {0};
	Nob_Cmd includes = args.includes;

	nob_cc(&cmd);
	for (int i = 0; i < includes.count; i++) {
		nob_cmd_append(&cmd, includes.items[i]);
	}
	nob_cmd_append(&cmd, "-MM", path, "-MF", "deps.d"); // TODO: put file in .cache or something
	if (!nob_cmd_run(&cmd)) return (Nob_Cmd){0};
	
	FILE* fp = fopen("deps.d", "rb");
	if (fp == NULL) {
		nob_log(NOB_ERROR, "Could not find the compiler generated dependency file %s: %s", "deps.d", strerror(errno));
		return (Nob_Cmd){0};
	}
	
	char c;
	while ((c = fgetc(fp)) != EOF && c != ':');
	if (c == EOF) {
		nob_log(NOB_ERROR, "Deps file does not contain semicolon");
		return (Nob_Cmd){0};
	}

	Nob_Cmd deps = {0};
	Nob_String_Builder builder = {0};

	while ((c = fgetc(fp)) != EOF) {
		if (builder.count == 0 && c == '\\') continue;
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			if (builder.count <= 0) continue;
			nob_sb_append_null(&builder);
			char* dep = nob_temp_strndup(builder.items, builder.count - 1);
			nob_cmd_append(&deps, dep);
			builder.count = 0; // Clear builder
			continue;
		}
		nob_sb_append_buf(&builder, &c, 1);
	}

	if (builder.count > 0) {
		nob_sb_append_null(&builder);
		char* dep = nob_temp_strndup(builder.items, builder.count - 1);
		nob_cmd_append(&deps, dep);
	}

	fclose(fp);
	nob_sb_free(builder);

	return deps;
}

// Error if path is null in returned object file.
NOBDEF Nob_Object_File nob_construct_obj(const char* path, Nob_Comp_Args args) {
	Nob_File_Type type = nob_get_file_type(path);
	if (type == -1) return (Nob_Object_File){NULL, NULL, {0}};
	switch (type) {
		case NOB_FILE_SYMLINK: 
		case NOB_FILE_OTHER: 
		case NOB_FILE_DIRECTORY: {
			nob_log(NOB_ERROR, "Cant construct object file for %s: Wrong file type", path);
			return (Nob_Object_File){NULL, NULL, {0}};
		}
		case NOB_FILE_REGULAR: ;
	}

	// TODO: replace file extension with .o
	char* bin_path = nob_temp_sprintf("%s/%s.o", args.opts.dest_path, path);

	return (Nob_Object_File){
		.path = bin_path,
		.source = path,
		.deps = nob_file_deps(path, args),
	};
}

NOBDEF void nob_objs_append(Nob_Object_Files *objs, Nob_Object_File obj) {
	if (objs->count >=objs->capacity) {
		objs->capacity = objs->capacity == 0 ? 4 : objs->capacity * 2;
		objs->items = NOB_REALLOC(objs->items, objs->capacity * sizeof(Nob_Object_File)); // TODO: memory leak
	}
	objs->items[objs->count] = obj;
	objs->count++;
}

NOBDEF bool nob_obj_should_build(Nob_Object_File obj) {
	if (!nob_file_exists(obj.path)) return true;

	Nob_Time_Stamp last_modified = nob_get_file_stat(obj.path).last_modified;

	for (int i = 0; i < obj.deps.count; i++) {
		if (!nob_file_exists(obj.deps.items[i])) {
			nob_log(NOB_ERROR, "Dependency %s does not exist", obj.deps.items[i]);
			return true;
		}

		Nob_File_Stat dep_stat = nob_get_file_stat(obj.deps.items[i]);

		if (nob_ts_is_newer(dep_stat.last_modified, last_modified)) {
			return true;
		}
	}
	return false;
}

NOBDEF Nob_Proc nob_build_object_async(Nob_Object_File obj, Nob_Comp_Args args, bool* skipped) {
	*skipped = false;

	char* ext = nob_temp_file_ext(obj.path);
	if (strcmp(ext, "c") != 0) return NOB_INVALID_PROC;

	if (!nob_obj_should_build(obj) && args.opts.build_type == NOB_BUILD_INCREMENTAL) {
		*skipped = true;
		return NOB_INVALID_PROC;
	}
	
	nob_log(NOB_INFO, "Building object %s", obj.path);


	Nob_Cmd cmd = {0};
	char* dir = nob_temp_dir_name(obj.path);

	#ifndef _WIN32 // TODO: swap with actual function
	nob_cmd_append(&cmd, "mkdir", "-p", dir);
	#else
	char* command = nob_temp_sprintf("%s %s %s", "New-Item -ItemType Directory -Path", dir, "-Force | Out-Null");
	nob_cmd_append(&cmd, "powershell", "-Command", command);
	#endif

	if (!nob_cmd_run_sync_and_reset(&cmd)) return NOB_INVALID_PROC;  // TODO: this is not very good

	nob_cc(&cmd);

	// Add flags
	for (int i = 0; i < args.flags.count; i++) {
		nob_cmd_append(&cmd, args.flags.items[i]);
	}

	// Add includes
	for (int i = 0; i < args.includes.count; i++) {
		nob_cmd_append(&cmd, args.includes.items[i]);
	}

	nob_cmd_append(&cmd, "-c");
	nob_cc_inputs(&cmd, obj.source);
	nob_cc_output(&cmd, obj.path);
	Nob_Proc proc = nob_cmd_run_async(cmd);

	return proc;
}

// Error if count or capacity is equal to SIZE_MAX.
NOBDEF Nob_Procs nob_build_objects_async(Nob_Object_Files objs, Nob_Comp_Args args) {
	Nob_Procs procs = {0};

	bool skipped = false;
	for (int i = 0; i < objs.count; i++) {
		Nob_Proc proc = nob_build_object_async(objs.items[i], args, &skipped);
		if (skipped) continue;
		if (proc == INVALID_PROC) {
			nob_procs_wait(procs); // wait for all previous processes to finish
			return (Nob_Procs){.items = NULL, SIZE_MAX, SIZE_MAX};
		}

		nob_procs_append(&procs, proc, nob_nprocs() + 1); // TODO: maybe not nprocs() + 1
	}

	return procs;
}

NOBDEF bool nob_build_object(Nob_Object_File obj, Nob_Comp_Args args) {
	bool skipped = false;
	Nob_Proc proc = nob_build_object_async(obj, args, &skipped);
	if (skipped) return true;
	if (proc == INVALID_PROC) return false;
	if (!nob_proc_wait(proc)) return false;
	return true;
}

NOBDEF bool nob_build_objects(Nob_Object_Files objs, Nob_Comp_Args args) {
	Nob_Procs procs = nob_build_objects_async(objs, args);
	if (procs.items == NULL && procs.count == SIZE_MAX) return false;

	if (!nob_procs_wait(procs)) return false;
	return true;
}



#endif
