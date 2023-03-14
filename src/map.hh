#ifndef MAP_HH_HEADER_GUARD
#define MAP_HH_HEADER_GUARD

#include <cstdlib> // std::size_t, std::memset
#include <cassert> // assert

#include <SDL2/SDL.h>

namespace reicaster {

enum class wall_type {
	bricks, // TODO: Make wall_type actually do something
};

struct tile {
	tile();
	tile(wall_type left, wall_type right, wall_type up, wall_type down, float h = 1, float z = 0);
	tile(wall_type type, float h = 1, float z = 0);

	wall_type left, right, up, down;
	bool      collision;
	float     z, h;
};

struct map {
	void create(std::size_t w, std::size_t h, const char *ascii_representation);
	void destroy();

	tile  at(std::size_t x, std::size_t y) const;
	tile &at(std::size_t x, std::size_t y);

	bool check_point_collides(float x, float y) const;
	bool check_point_collides(float x, float y, float z) const;

	tile       *raw;
	std::size_t w, h, size;
};

}

#endif
