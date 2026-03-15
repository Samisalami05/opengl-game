/*
**
**
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

#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#include <stdio.h>
#endif

#ifdef _WIN32

#else
#	include <sys/mman.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "nob.h"

// Is a non existent proc, does not indicate a error
#define NOB_NONEXISTING_PROC (Nob_Proc)-1

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
NOBDEF Nob_Fd nob_fd_create(const char* name);

NOBDEF Nob_Cmd nob_file_deps(const char* path, Nob_Comp_Args args);
NOBDEF Nob_Object_File nob_construct_obj(const char* path, Nob_Comp_Args args);
NOBDEF Nob_Object_Files nob_construct_objs(Nob_File_Paths paths, Nob_Comp_Args args);
NOBDEF bool nob_obj_should_build(Nob_Object_File obj);


NOBDEF bool nob_file_walk(Nob_Walk_Entry entry);
NOBDEF bool nob_file_walk_rec(Nob_Walk_Entry entry);
NOBDEF bool nob_dir_files(const char* path, Nob_File_Paths* files);
NOBDEF bool nob_dir_files_rec(const char* path, Nob_File_Paths* files);

// TODO: async is deprecated, use cmd opts
NOBDEF Nob_Proc nob_build_object_async(Nob_Object_File obj, Nob_Comp_Args args);
NOBDEF Nob_Procs nob_build_objects_async(Nob_Object_Files objs, Nob_Comp_Args args);
NOBDEF bool nob_build_object(Nob_Object_File obj, Nob_Comp_Args args);
NOBDEF bool nob_build_objects(Nob_Object_Files objs, Nob_Comp_Args args);




#ifdef NOB_IMPLEMENTATION

NOBDEF bool nob_procs_append(Nob_Procs *procs, Nob_Proc proc, size_t max_procs_count)
{
    nob_da_append(procs, proc);

	if (max_procs_count <= 0) max_procs_count = (size_t)nob_nprocs() + 1;

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

// Creates a virtual file descriptor
NOBDEF Nob_Fd nob_fd_create(const char* name) {
#ifdef _WIN32
	// TODO: do windows implementation
	SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;

    Nob_Fd fd = CreateFile(
                    name,
                    GENERIC_READ,
                    0,
                    &saAttr,
                    CREATE_NEW,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);

    if (fd == INVALID_HANDLE_VALUE) {
        nob_log(NOB_ERROR, "Could not open file %s: %s", path, nob_win32_error_message(GetLastError()));
        return NOB_INVALID_FD;
    }

    return fd;
#else
	int fd = memfd_create(name, 0);
	if (fd < 0) {
		nob_log(NOB_ERROR, "Could not create virtual file descriptor: %s", strerror(errno));
		return NOB_INVALID_FD;
	}
	return fd;
#endif
}

static Nob_Cmd parse_deps(FILE* fp) {
	char c;
	while ((c = fgetc(fp)) != EOF && c != ':');
	if (c == EOF) {
		nob_log(NOB_ERROR, "Invalid dependency file format: File does not contain a target");
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
	nob_sb_free(builder);

	return deps;
}


// TODO: do async
NOBDEF Nob_Cmd nob_file_deps(const char* path, Nob_Comp_Args args) {
	Nob_Cmd cmd = {0};
	Nob_Cmd includes = args.includes;

	char* fd_name = nob_temp_sprintf("nob_%s_deps.d", path);
	Nob_Fd fd = nob_fd_create(fd_name);
	if (fd == NOB_INVALID_FD) return (Nob_Cmd){0};

	nob_cc(&cmd);
	for (int i = 0; i < includes.count; i++) {
		nob_cmd_append(&cmd, includes.items[i]);
	}
	nob_cmd_append(&cmd, "-MM", path);


	Nob_Cmd_Opt opts = {0};
	Nob_Proc proc = nob__cmd_start_process(cmd, NULL, &fd, &fd);
	if (proc == NOB_INVALID_PROC) return (Nob_Cmd){0};
	if (!nob_proc_wait(proc)) return (Nob_Cmd){0};

#ifdef _WIN32
	_lseek(fd, 0, SEEK_SET);
	FILE* fp = _fdopen(fd, "rb");
#else
	lseek(fd, 0, SEEK_SET);
	FILE* fp = fdopen(fd, "rb");
#endif

	if (fp == NULL) {
		nob_log(NOB_ERROR, "Could not find the dependency file descriptor: %s", strerror(errno));
		fclose(fp);
		return (Nob_Cmd){0};
	}

	Nob_Cmd deps = parse_deps(fp);
	fclose(fp);
	nob_fd_close(fd);

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

NOBDEF Nob_Object_Files nob_construct_objs(Nob_File_Paths paths, Nob_Comp_Args args) {
	Nob_Object_Files objs = {0};
	for (int i = 0; i < paths.count; i++) {
		Nob_Object_File obj = nob_construct_obj(paths.items[i], args);
		if (obj.path == NULL) continue; // TODO: maybe return error
		nob_da_append(&objs, obj);
	}

	return objs;
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

NOBDEF bool nob_file_walk(Nob_Walk_Entry entry) {
	if (entry.level > 1) return false;
	return nob_file_walk_rec(entry);
}

NOBDEF bool nob_file_walk_rec(Nob_Walk_Entry entry) {
	switch (entry.type) {
		case NOB_FILE_DIRECTORY:
		case NOB_FILE_OTHER: return true;
		default: ;
	}

	Nob_File_Paths* files = entry.data;
	nob_da_append(files, nob_temp_strdup(entry.path));
	return true;
}

// Does not search in sub directories, the given path can be a file.
NOBDEF bool nob_file_search(const char* path, Nob_File_Paths* files) {
	Nob_Walk_Dir_Opt opt = {
		.data = files,
		.post_order = false,
	};
	return nob_walk_dir_opt(path, nob_file_walk, opt); 
}

// Searches the whole directory and its subdirectories, the given path can be a file.
NOBDEF bool nob_file_search_rec(const char* path, Nob_File_Paths* files) {
	Nob_Walk_Dir_Opt opt = {
		.data = files,
		.post_order = false,
	};
	return nob_walk_dir_opt(path, nob_file_walk_rec, opt); 
}

// Returns NOB_NONEXISTING_PROC if object did not have to compile
NOBDEF Nob_Proc nob_build_object_async(Nob_Object_File obj, Nob_Comp_Args args) {
	char* ext = nob_temp_file_ext(obj.source);
	if (strcmp(ext, ".c") != 0) return NOB_INVALID_PROC;

	if (!nob_obj_should_build(obj) && args.opts.build_type == NOB_BUILD_INCREMENTAL) {
		return NOB_NONEXISTING_PROC;
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

	for (int i = 0; i < objs.count; i++) {
		Nob_Proc proc = nob_build_object_async(objs.items[i], args);
		if (proc == NOB_NONEXISTING_PROC) continue;
		if (proc == INVALID_PROC) {
			nob_procs_wait(procs); // wait for all previous processes to finish
			return (Nob_Procs){.items = NULL, SIZE_MAX, SIZE_MAX};
		}

		nob_procs_append(&procs, proc, 0); // TODO: maybe not nprocs() + 1
	}

	return procs;
}

NOBDEF bool nob_build_object(Nob_Object_File obj, Nob_Comp_Args args) {
	Nob_Proc proc = nob_build_object_async(obj, args);
	if (proc == NOB_NONEXISTING_PROC) return true;
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
