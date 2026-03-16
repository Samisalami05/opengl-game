#define NOB_IMPLEMENTATION
//#define NOB_NO_ECHO
#include "nob_ext.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define BUILD_FOLDER "build"
#define SRC_FOLDER "src"
#define SRC_EXTENSION ".c"
#define NAME "main"

// Is a .o object file
typedef struct {
    const char* path;
	Nob_Cmd deps;
} object;

typedef enum {
	BUILD_CLEAN,
	BUILD_INCREMENTAL,
} build_type;

typedef struct {
	const char* file_extenstion;
	const char** flags; // Null terminated
	build_type type;
	const char* source_path;
	const char* dest_path;
} comp_opts;

typedef struct {
	Nob_Procs procs;
	Nob_Cmd objects;
	bool error;
} comp_data;

typedef struct {
	comp_opts opts;
	comp_data data;
	Nob_Cmd includes;
} comp_args;

typedef struct {
	const char* name;
	const char* path;
	const char** links;
	const char* include_path;
	const char** cmd;

	// Linking
	// -l name  or  -L path/to/name.a
	
	// Include
	// -I path/to/name
} external_lib;

typedef struct {
	external_lib* libs;
	size_t count;
	size_t capacity;
} libraries;

typedef struct {
	const char* build_dir;
const char* src_dir;
	libraries libs;
} project;

//
//
// target glfw = construct_lib("glfw");
// target_include_dir(glfw, "path/to/include");
// target_link("-lglfw");
// target_set_path("path/to/glfw");
//
// add_lib(&proj, glfw);




// ---- Util ----
uint8_t strcmpe(const char* str, const char* end, int str_len);
object construct_object(const char* path, comp_opts opts, Nob_Cmd includes);
bool object_should_build(object obj);
Nob_Cmd file_deps(const char* path, Nob_Cmd includes);
bool walk_func(Nob_Walk_Entry entry);

// ---- API ----
bool build_project(project proj, build_type type);
comp_data build_objects(comp_opts opts, libraries libs);
comp_data build_libs(libraries libs, comp_opts opts);
bool link_objects(comp_data* data, int count, libraries libs);
void add_external_lib(libraries* libs, external_lib lib);
Nob_Cmd lib_includes(libraries libs);
Nob_Cmd lib_links(libraries libs);



int main(int argc, char* argv[]) {
	nob_set_log_handler(nob_cancer_log_handler);
	NOB_GO_REBUILD_URSELF(argc, argv);


	Nob_Comp_Opts opts = {
		.build_type = NOB_BUILD_INCREMENTAL,
		.name = NULL,
		.dest_path = "build",
	};

	Nob_Comp_Args args = {
		.opts = opts,
		.includes = {0},
		.links = {0},
		.flags = {0},
	};

	Nob_Target main = nob_construct_target("main", "src/main.c", NOB_COMP_EXECUTABLE);
	nob_target_include(&main, "wow", "ayo");
	

	Nob_Target engine = nob_construct_target("engine", "src/engine", NOB_COMP_SHARED);
	Nob_Target glfw = nob_construct_target("glfw", "libs/glfw-3.4", NOB_COMP_CMD);
	nob_target_cmd_append(&glfw, "echo", "building", "glfw");
	Nob_Target glad = nob_construct_target("glad", "libs/glad", NOB_COMP_OBJECT);
	Nob_Target assimp = nob_construct_target("assimp", "libs/assimp", NOB_COMP_CMD);
	nob_target_cmd_append(&assimp, "echo", "building", "assimp");
	
	nob_target_dependency(&main, &engine);
	nob_target_dependency(&engine, &glfw, &glad, &assimp);

	nob_build_target(main, opts);


	/*
	Nob_Target main = nob_construct_target("main", "src/main.c", NOB_COMP_EXECUTABLE);
	nob_target_include(main, "src/engine");
	nob_target_dependency(main, engine);

	Nob_Target engine = nob_construct_target("engine", "src/engine", NOB_COMP_SHARED);
	nob_target_link(&main, engine);
	nob_target_dependency(engine, glfw);


	// GLFW
	Nob_Target glfw = nob_construct_target("glfw", "libs/glfw-3.4", NOB_COMP_CMD);
	nob_target_add_cmd(glfw, "cmake", "-B", "build");
	nob_target_add_links(glfw, "-lglfw");


	nob_build_target(main);
	*/

	return 0;

	nob_cmd_append(&args.includes, "-Isrc/engine", "-Ilibs/glad/include", "-Ilibs");

	
	Nob_File_Paths files = {0};
	if (!nob_file_search_rec("src", &files, ".c")) return 1;

	Nob_Object_Files objs = nob_construct_objs(files, args);

	if (!nob_build_objects(objs, args)) return 1;

	if (!nob_link_objects(objs, args)) return 1;














	return 0;

	project proj = {
		.build_dir = BUILD_FOLDER,
		.src_dir = SRC_FOLDER,
		.libs = {0},
	};

	add_external_lib(&proj.libs, (external_lib) {
		.name = "engine",
		.cmd = NULL,
		.include_path = "-Isrc/engine",
		.path = NULL,
		.links = NULL,
	});

	add_external_lib(&proj.libs, (external_lib) {
		.name = "libs",
		.cmd = NULL,
		.include_path = "-Ilibs",
		.path = NULL,
		.links = (const char*[]){"-Llibs", NULL},
	});

	add_external_lib(&proj.libs, (external_lib) {
		.name = "math",
		.cmd = NULL,
		.include_path = NULL,
		.path = NULL,
		.links = (const char*[]){"-lm", NULL},
	});

	add_external_lib(&proj.libs, (external_lib) {
		.name = "glfw-3.4",
		.cmd = (const char*[]){"cmake", "-B", "build", "&&", "cmake", "--build", "build", NULL},
		.include_path = "-Ilibs/glfw-3.4/include",
		.path = "libs/glfw-3.4",
		.links = (const char*[]){"-lglfw", NULL},
	});

	add_external_lib(&proj.libs, (external_lib) {
		.name = "glad",
		.cmd = NULL,
		.include_path = "-Ilibs/glad/include",
		.path = "libs/glad/glad.c",
		.links = NULL,
	});

	add_external_lib(&proj.libs, (external_lib) {
		.name = "assimp",
		.cmd = (const char*[]){"cmake", "CMakeLists.txt", "&&", "cmake", "--build", ".", NULL},
		.include_path = "-Ilibs/assimp/include",
		.path = "libs/assimp",
		.links = (const char*[]){"-lassimp", NULL},

	});
	
	add_external_lib(&proj.libs, (external_lib) {
		.name = "opengl",
		.cmd = NULL,
		.include_path = NULL,
		.path = NULL,
		.links = (const char*[]){"-lGL", "-ldl", NULL},
	});




	build_project(proj, BUILD_INCREMENTAL);

	return 0;
}







static void nob_cmd_add_strs(Nob_Cmd* cmd, const char** strs) {
	if (strs == NULL) return;
	while (*strs != NULL) {
		nob_cmd_append(cmd, *strs);
		strs++;
	}
}

Nob_Cmd lib_links(libraries libs) {
	Nob_Cmd links = {0};
	for (int i = 0; i < libs.count; i++) {
		external_lib lib = libs.libs[i];
		if (lib.links == NULL) continue;
		
		nob_cmd_add_strs(&links, lib.links);
	}
	return links;
}

Nob_Cmd lib_includes(libraries libs) {
	Nob_Cmd includes = {0};
	for (int i = 0; i < libs.count; i++) {
		if (libs.libs[i].include_path != NULL) {
			nob_cmd_append(&includes, libs.libs[i].include_path);
		}
	}
	return includes;
}

bool build_library(external_lib lib) {
	if (lib.path == NULL) return true;
	if (lib.cmd != NULL && *lib.cmd != NULL) {
		Nob_Cmd cmd = {0};
		// pushd libs/assimp/ >/dev/null && 
		// cmake -B build && popd >/dev/null
		// /
		// TODO: support for windows
		//nob_cmd_append(&cmd, "pushd", lib.path, ">/dev/null", "&&");
		Nob_String_Builder builder = {0}; // TODO: dont use bash, use chdir with execvp
		nob_sb_append_cstr(&builder, "pushd ");
		nob_sb_append_cstr(&builder, lib.path);
		nob_sb_append_cstr(&builder, " && ");

		const char** build_cmd = lib.cmd;
		while (*build_cmd != NULL) {
			nob_sb_append_cstr(&builder, *build_cmd);
			nob_sb_append_buf(&builder, " ", 1);
			build_cmd++;
		}
		nob_sb_append_cstr(&builder, " && popd");
		nob_sb_append_null(&builder);
		//nob_cmd_append(&cmd, "&&", "popd", ">/dev/null");
		
		nob_cmd_append(&cmd, "bash", "-c", builder.items);
		if (!nob_cmd_run(&cmd)) return false;
		return true;
	}
	return true;
}

void nob_cmd_append_cmd(Nob_Cmd* cmd, const Nob_Cmd other) {
	for (int i = 0; i < other.count; i++) {
		nob_cmd_append(cmd, other.items[i]);
	}
}

comp_data build_libs(libraries libs, comp_opts opts) {
	const char* dest_path = opts.dest_path;

	comp_data out = {0};

	for (int i = 0; i < libs.count; i++) {
		external_lib lib = libs.libs[i];
		nob_log(NOB_INFO, "Building library %s\n", lib.name);

		if (!build_library(lib)) {
			nob_log(NOB_ERROR, "Could not build library %s", lib.name);
			return (comp_data){.objects = (Nob_Cmd){0}, .procs = (Nob_Procs){0}, .error = 1};
		}

		if (lib.path == NULL) {
			nob_log(NOB_WARNING, "No path supplied to library %s with build system: Cant initiate build", lib.name);
			continue;
		}

		if (lib.cmd == NULL || *lib.cmd == NULL) {
			// Add to project compilation
			opts.source_path = lib.path;
			printf("building %s to %s\n", lib.name, opts.dest_path);

			comp_data data = build_objects(opts, libs); // TODO: should libs be sent?
			if (data.error) return (comp_data){.objects = (Nob_Cmd){0}, .procs = (Nob_Procs){0}, .error = 1};
			nob_procs_wait(data.procs);
			nob_cmd_append_cmd(&out.objects, data.objects);
		}
	}
	return out;
}

void add_external_lib(libraries* libs, external_lib lib) {
	if (libs->count >=libs->capacity) {
		libs->capacity = libs->capacity == 0 ? 4 : libs->capacity * 2;
		libs->libs = NOB_REALLOC(libs->libs, libs->capacity * sizeof(external_lib)); // TODO: memory leak
	}
	libs->libs[libs->count] = lib;
	libs->count++;
}

bool link_objects(comp_data* data, int count, libraries libs) {
	if (count == 0 || (count >= 1 && data[0].objects.count == 0)) {
		nob_log(NOB_ERROR, "No objects provided to linking");
		return false;
	}
	Nob_Cmd cmd = {0};
	nob_cc(&cmd);

	//Nob_Cmd includes = lib_includes(libs);
	//for (int i = 0; i < includes.count; i++) {
	//	nob_cmd_append(&cmd, includes.items[i]);
	//}

	for (int i = 0; i < count; i++) {
		if (data[i].error) continue;
		nob_procs_wait(data[i].procs);

		Nob_Cmd objs = data[i].objects;
		for (int i = 0; i < objs.count; i++) {
			nob_cmd_append(&cmd, objs.items[i]);
		}
	}

	nob_cc_output(&cmd, NAME);

	Nob_Cmd links = lib_links(libs);
	for (int i = 0; i < links.count; i++) {
		nob_cmd_append(&cmd, links.items[i]);
	}

	if (!nob_cmd_run(&cmd)) return false;
	return true;
}

comp_data build_objects(comp_opts opts, libraries libs) {
	if (!nob_file_exists(opts.source_path)) {
		nob_log(NOB_ERROR, "Cant build objects: The source folder '%s' does not exist", opts.source_path);
		return (comp_data){.objects = {0}, .procs = {0}, .error = 1};
	}
	comp_args args = {.opts = opts, .data = {0}, .includes = lib_includes(libs)};

	nob_mkdir_if_not_exists(opts.dest_path);

	Nob_Walk_Dir_Opt opt = {
		.data = &args,
		.post_order = false,
	};
	if (!nob_walk_dir_opt(opts.source_path, walk_func, opt)) return (comp_data){.objects = {0}, .procs = {0}, .error = 1};
	return args.data;
}

bool build_failed() {
	nob_log(NOB_ERROR, "BUILD FAILED");
	return false;
}	

bool build_project(project proj, build_type type) {
	nob_log(NOB_INFO, "--- BUILDING LIBS ---");

	comp_opts opts = {
		.flags = (const char*[]) {NULL},
		.file_extenstion = ".c",
		.type = type,
		.source_path = proj.src_dir,
		.dest_path = proj.build_dir,
	};

	comp_data data[2] = {0};
	data[0] = build_libs(proj.libs, opts);
	if (data[0].error) return build_failed();


	nob_log(NOB_INFO, "--- BUILDING SOURCES ---");

	data[1] = build_objects(opts, proj.libs);
	if (data[1].error) return build_failed();

	nob_log(NOB_INFO, "--- LINKING EXECUTABLE ---");

	if (!link_objects(data, 2, proj.libs)) return build_failed();
	return true;
}

Nob_Cmd file_deps(const char* path, Nob_Cmd includes) {
	Nob_Cmd cmd = {0};

	nob_cc(&cmd);
	for (int i = 0; i < includes.count; i++) {
		nob_cmd_append(&cmd, includes.items[i]);
	}
	nob_cmd_append(&cmd, "-MM", path, "-MF", "deps.d");
	if (!nob_cmd_run(&cmd)) return (Nob_Cmd){0}; // TODO: do async
	
	FILE* fp = fopen("deps.d", "rb");
	
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

object construct_object(const char* path, comp_opts opts, Nob_Cmd includes) {
	// TODO: replace file extension with .o
	char* bin_path = nob_temp_sprintf("%s/%s.o", opts.dest_path, path);

	return (object){
		.path = bin_path,
		.deps = file_deps(path, includes),
	};
}

uint8_t strcmpe(const char* str, const char* end, int str_len) {
	unsigned long end_len = strlen(end);
	if (str_len < end_len) return 0;
	return strcmp(str + str_len - end_len, end) == 0;
}

bool object_should_build(object obj) {
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

bool walk_func(Nob_Walk_Entry entry) {
	switch (entry.type) {
		case NOB_FILE_SYMLINK: printf("SYMLINK\n");
		case NOB_FILE_OTHER: printf("OTHER\n");
		case NOB_FILE_DIRECTORY: return true;
		case NOB_FILE_REGULAR: ;
	}

	comp_args* args = entry.data;
	comp_opts opts = args->opts;
	comp_data* data = &args->data;

	unsigned long len = strlen(entry.path);
	if (!strcmpe(entry.path, opts.file_extenstion, len)) return true;

	object obj = construct_object(entry.path, opts, args->includes);

	nob_cmd_append(&data->objects, obj.path);

	if (!object_should_build(obj) && opts.type == BUILD_INCREMENTAL) {
		nob_log(NOB_INFO, "Skipping object %s", obj.path);
		return true;
	}

	nob_log(NOB_INFO, "Building object %s", obj.path);

	Nob_Cmd cmd = {0};
	char* dir = nob_temp_dir_name(obj.path);
	
	#ifndef _WIN32
	nob_cmd_append(&cmd, "mkdir", "-p", dir);
	#else
	char* command = nob_temp_sprintf("%s %s %s", "New-Item -ItemType Directory -Path", dir, "-Force | Out-Null");
	nob_cmd_append(&cmd, "powershell", "-Command", command);
	#endif
	if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;  // TODO: this is not very good

	Nob_Cmd_Opt cmd_opt = {
		.async = &data->procs,
		.max_procs = 0,
		.dont_reset = false,
		.stdin_path = 0,
		.stdout_path = 0,
		.stderr_path = 0,
	};

	nob_cc(&cmd);
	while (*opts.flags != NULL) {
		nob_cmd_append(&cmd, *opts.flags);
		opts.flags++;
	}

	for (int i = 0; i < args->includes.count; i++) {
		nob_cmd_append(&cmd, args->includes.items[i]);
	}

	nob_cmd_append(&cmd, "-c");
	nob_cc_inputs(&cmd, entry.path);
	nob_cc_output(&cmd, obj.path);
	if (!nob_cmd_run_opt(&cmd, cmd_opt)) return false;

	return true;
}
