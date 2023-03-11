#ifndef PIXELS_HH_HEADER_GUARD
#define PIXELS_HH_HEADER_GUARD

#include <cstdlib> // std::size_t, std::memset

#include <SDL2/SDL.h>

namespace ray3d {

inline Uint32 color_to_pixel_abgr(Uint8 r, Uint8 g, Uint8 b, Uint8 a = SDL_ALPHA_OPAQUE) {
	return r | (g << 8) | (b << 16) | (a << 24);
}

struct pixels {
	void create(std::size_t w, std::size_t h);
	void destroy();
	void clear();

	Uint32  at(int x, int y) const;
	Uint32 &at(int x, int y);
	bool    check_point_bounds(int x, int y);

	Uint32     *raw;
	std::size_t w, h, size;
};

}

#endif
