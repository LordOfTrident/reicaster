#ifndef SURFACE_SHEET_HH_HEADER_GUARD
#define SURFACE_SHEET_HH_HEADER_GUARD

#include <SDL2/SDL.h>

namespace reicaster {

constexpr std::size_t max_ids = 128;

class surface_sheet {
public:
	surface_sheet();
	void create(SDL_Surface *sheet, int tile_size);

	void get_color_at(std::size_t id, int x, int y, int &r, int &g, int &b);

	int get_tile_size() const;

private:
	vec2i id_to_pos[max_ids];

	int tile_size, tiles_in_row;
	SDL_Surface *s;
};

}

#endif
