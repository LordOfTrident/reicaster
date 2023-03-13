#include <string.h> /* strcmp, strcpy, strlen */
#include <malloc.h> /* malloc, free */
#include <assert.h> /* assert */

#define SRC "src"
#define BIN "bin"
#define RES "res"

#define IMPLEMENTATIONS "implementations.cc"
#define MAIN "main.cc"

#define OUT     "app"
#define INSTALL "reicaster"

#define CARGS "-O2", "-std=c++17", "-Wall", "-Wextra", "-Werror", "-pedantic", \
              "-Wno-deprecated-declarations", "-I./chol"
#define CLIBS "-lSDL2", "-lSDL2main"

#define INSTALL_DIR "/usr/bin"

#define CBUILDER_IMPLEMENTATION
#include "chol/cbuilder.h"

#include "embeds.h"

char *cxx = CXX;

void uninstall(void) {
#ifdef BUILD_PLATFORM_WINDOWS
	LOG_FATAL("'uninstall' is not supported on Windows yet");
#else
	if (!fs_exists(INSTALL_DIR"/"INSTALL))
		LOG_FATAL("Failed to uninstall (is it installed?)");

	if (fs_remove_file(INSTALL_DIR"/"INSTALL) != 0)
		LOG_FATAL("Failed to uninstall (could not remove '%s', do you have permissions?)",
		          INSTALL_DIR"/"INSTALL);

	LOG_INFO("Uninstalled from '%s'", INSTALL_DIR"/"INSTALL);
#endif
}

void install(void) {
#ifdef BUILD_PLATFORM_WINDOWS
	LOG_FATAL("'install' is not supported on Windows yet");
#else
	if (!fs_exists(BIN"/"OUT))
		LOG_FATAL("Please compile before installing");

	if (fs_copy_file(BIN"/"OUT, INSTALL_DIR"/"INSTALL) != 0)
		LOG_FATAL("Failed to install (could not copy to '%s', do you have permissions?)",
		          INSTALL_DIR"/"INSTALL);

	LOG_INFO("Installed to '%s'", INSTALL_DIR"/"INSTALL);
#endif
}

char *build_source(build_cache_t *c, const char *src_name, bool force_rebuild) {
	char *out_name = fs_replace_ext(src_name, "o");

	char *out = FS_JOIN_PATH(BIN, out_name);
	char *src = FS_JOIN_PATH(SRC, src_name);

	free(out_name);

	int64_t m_cached = build_cache_get(c, src);
	int64_t m_now;
	if (fs_time(src, &m_now, NULL) != 0)
		LOG_FATAL("Could not get last modified time of '%s'", src);


	if (m_cached != m_now || force_rebuild) {
		build_cache_set(c, src, m_now);
		CMD(cxx, "-c", src, "-o", out, CARGS);
	}

	free(src);
	return out;
}

bool create_embeds(build_cache_t *c) {
	bool force_rebuild = false;

	if (!fs_exists("src/embeds"))
		fs_create_dir("src/embeds");

	FOREACH_IN_ARRAY(embeds, const char*, ARRAY_SIZE(embeds), name, {
		char *header_path = FS_JOIN_PATH(SRC, *name);
		char *tmp         = FS_JOIN_PATH(RES, fs_basename(*name));
		char *res_path    = fs_remove_ext(tmp);
		free(tmp);

		int64_t m_cached = build_cache_get(c, res_path);
		int64_t m_now;
		if (fs_time(res_path, &m_now, NULL) != 0)
			LOG_FATAL("Could not get last modified time of '%s'", res_path);

		if (m_cached != m_now) {
			if (!force_rebuild)
				force_rebuild = true;

			build_cache_set(c, res_path, m_now);
			embed(res_path, header_path, BYTE_ARRAY);
		}

		free(res_path);
		free(header_path);
	});

	return force_rebuild;
}

void build(void) {
	if (!fs_exists("bin"))
		fs_create_dir("bin");

	build_cache_t c;
	if (build_cache_load(&c) != 0)
		LOG_FATAL("Build cache is corrupted");

	bool force_rebuild = create_embeds(&c);

	char  *o_files[128];
	size_t o_files_count = 0;

	o_files[o_files_count ++] = build_source(&c, IMPLEMENTATIONS, false);

	int  status;
	FOREACH_IN_DIR(SRC, dir, ent, {
		const char *ext = fs_ext(ent.name);
		if (strcmp(ext, "hh") != 0 && strcmp(ext, "cc") != 0)
			continue;

		char *src = FS_JOIN_PATH(SRC, ent.name);

		int64_t m_cached = build_cache_get(&c, src);
		int64_t m_now;
		if (fs_time(src, &m_now, NULL) != 0)
			LOG_FATAL("Could not get last modified time of '%s'", src);

		if (m_cached != m_now) {
			if (!force_rebuild)
				force_rebuild = true;

			build_cache_set(&c, src, m_now);
		}

		free(src);
	}, status);

	if (status != 0)
		LOG_FATAL("Failed to open directory '%s'", SRC);

	o_files[o_files_count ++] = build_source(&c, MAIN, force_rebuild);

	if (o_files_count == 0)
		LOG_INFO("Nothing to rebuild");
	else {
		if (build_cache_save(&c) != 0)
			LOG_FATAL("Failed to save build cache");

		COMPILE(cxx, o_files, o_files_count, "-o", BIN"/"OUT, CARGS, CLIBS);

		for (size_t i = 0; i < o_files_count; ++ i)
			free(o_files[i]);
	}

	build_cache_free(&c);
}

int main(int argc, const char **argv) {
	args_t a = build_init(argc, argv);
	/* Add the 'clean' subcommand to the usage */
	build_set_usage("[clean | install | uninstall] [OPTIONS]");

	flag_cstr(NULL, "CXX", "The C++ compiler path", &cxx);

	args_t stripped; /* The flagless arguments */
	build_parse_args(&a, &stripped);

	const char *subcmd = args_shift(&a);
	if (subcmd != NULL) {
		if (a.c > 0) {
			build_arg_error("Unexpected argument '%s' for '%s'", a.v[0], subcmd);
			exit(EXIT_FAILURE);
		}

		if (strcmp(subcmd, "clean") == 0) {
			build_clean(BIN);

			if (fs_exists(BIN"/"OUT))
				fs_remove_file(BIN"/"OUT);
		} else if (strcmp(subcmd, "install") == 0)
			install();
		else if (strcmp(subcmd, "uninstall") == 0)
			uninstall();
		else {
			build_arg_error("Unknown subcommand '%s'", subcmd);
			exit(EXIT_FAILURE);
		}
	} else
		build();

	free(stripped.base);
	return EXIT_SUCCESS;
}
