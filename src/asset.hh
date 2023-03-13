#ifndef ASSET_HH_HEADER_GUARD
#define ASSET_HH_HEADER_GUARD

#include <cstdlib> // std::size_t

#include <SDL2/SDL.h>

#include <clog.h>

namespace reicaster {

struct asset {
	static asset create_from(SDL_Renderer *r, Uint8 *bytes, std::size_t size,
	                         bool create_texture = true);
	static asset create_from(SDL_Renderer *r, const char *path, bool create_texture = true);

	void load(SDL_Renderer *r, Uint8 *bytes, std::size_t size, bool create_texture = true);
	void load(SDL_Renderer *r, const char *path, bool create_texture = true);
	void destroy();

	SDL_Surface *s;
	SDL_Texture *t;
};

}

#endif
