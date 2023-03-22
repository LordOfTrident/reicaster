#ifndef MAP_HH_HEADER_GUARD
#define MAP_HH_HEADER_GUARD

#include <cstdlib> // std::size_t, std::memset
#include <cassert> // assert

#include <SDL2/SDL.h>

#include "common.hh"

namespace reicaster {

namespace wall {
	enum id {
		bricks,
		bricks_with_paint,
	};
}

struct tile {
	tile();
	tile(wall::id left, wall::id right, wall::id up, wall::id down, float h = 1, float z = 0);
	tile(wall::id id, float h = 1, float z = 0);

	wall::id &id(direction side);
	wall::id  id(direction side) const;

	wall::id ids[4];
	bool     collision;
	float    z, h;
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
