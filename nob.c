#define NOB_IMPLEMENTATION
//#define NOB_NO_ECHO
#include "nob_ext.h"

#include <stddef.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
	nob_set_log_handler(nob_cancer_log_handler);
	NOB_GO_REBUILD_URSELF(argc, argv);

	Nob_Comp_Opts opts = {
		.build_type = NOB_BUILD_INCREMENTAL,
		.name = NULL,
		.dest_path = "build",
	};

	Nob_Target main = nob_construct_target("main", "src/main.c", NOB_COMP_EXECUTABLE);
	

	Nob_Target engine = nob_construct_target("engine", "src/engine", NOB_COMP_OBJECT);
	nob_target_include(&engine, "-Isrc/engine", "-Ilibs");
	nob_target_link(&engine, "-Llibs", "-lm", "-lGL", "-ldl", "-lglfw", "-lassimp");
	
	// Glfw
	Nob_Target glfw = nob_construct_target("glfw", "libs/glfw-3.4", NOB_COMP_CMD);
	nob_target_cmd_append(&glfw, "cmake", "-B", "build", "&&", "cmake", "--build", "build");

	// Glad
	Nob_Target glad = nob_construct_target("glad", "libs/glad", NOB_COMP_OBJECT);
	nob_target_include(&glad, "-Ilibs/glad/include");

	// Assimp
	Nob_Target assimp = nob_construct_target("assimp", "libs/assimp", NOB_COMP_CMD);
	nob_target_cmd_append(&assimp, "cmake", "CMakeLists.txt", "&&", "cmake", "--build", ".");
	
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
}
