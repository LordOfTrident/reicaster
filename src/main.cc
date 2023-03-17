#include <iostream> // std::cerr, std::cout
#include <cstdlib>  // std::exit, EXIT_SUCCESS, EXIT_FAILURE
#include <cassert>  // assert

#include <chol/args.h>

#include "app.cc"
#include "common.cc"
#include "pixels.cc"
#include "map.cc"
#include "math.cc"
#include "raycast.cc"
#include "entity.cc"
#include "asset.cc"

#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_PATCH 1

constexpr auto name    = "reicaster";
constexpr auto fps_cap = 60;

const char *exec_path;

void usage() {
	args_print_usage(stdout, exec_path, "[OPTIONS]");
	std::exit(EXIT_SUCCESS);
}

void version() {
	std::cout << reicaster::fmt("%v v%v.%v.%v", name, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)
	          << std::endl;
	std::exit(EXIT_SUCCESS);
}

std::string args_error_to_str(args_t &a, int err, int where) {
	switch (err) {
	case ARG_OUT_OF_MEM: assert(0 && "malloc() fail");

	case ARG_UNKNOWN:       return reicaster::fmt("Unknown flag %v",               a.v[where]);
	case ARG_MISSING_VALUE: return reicaster::fmt("Flag '%v' has a missing value", a.v[where]);
	default:                return reicaster::fmt("Incorrect type for flag '%v'",  a.v[where]);
	}
}

void parse_args(int argc, const char **argv) {
	args_t a  = new_args(argc, argv);
	exec_path = args_shift(&a);

	bool help = false, ver = false;
	flag_bool("h", "help",    "Show the usage",   &help);
	flag_bool("v", "version", "Show the version", &ver);

	int where;
	int err = args_parse_flags(&a, &where, NULL);
	if (err != ARG_OK) {
		std::cerr << reicaster::fmt("Error: %v\nTry '%v -h'",
		                            args_error_to_str(a, err, where), exec_path) << std::endl;
		std::exit(EXIT_FAILURE);
	}

	if (help)
		usage();
	else if (ver)
		version();
}

int main(int argc, const char **argv) {
	parse_args(argc, argv);

	auto app = reicaster::application::create(name);

	while (app->is_running()) {
		app->render();
		app->update();

		SDL_Delay(1000.0 / fps_cap); // TODO: Better frame delay
	}

	delete app;
	return EXIT_SUCCESS;
}
