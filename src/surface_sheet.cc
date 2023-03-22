#include "surface_sheet.hh"

namespace reicaster {

surface_sheet::surface_sheet(): s(nullptr) {}

void surface_sheet::create(SDL_Surface *sheet, int tile_size) {
	s = sheet;

	this->tile_size = tile_size;
	tiles_in_row    = s->w / tile_size;

	/* Precalculate ID positions (optimization) */
	for (std::size_t id = 0; id < max_ids; ++ id) {
		id_to_pos[id].x = id % tiles_in_row * tile_size;
		id_to_pos[id].y = id / tiles_in_row * tile_size;
	}
}

void surface_sheet::get_color_at(std::size_t id, int x, int y, int &r, int &g, int &b) {
	int surface_x = id_to_pos[id].x + x;
	int surface_y = id_to_pos[id].y + y;

	auto pixel = static_cast<Uint32*>(s->pixels)[(surface_y * s->w) + surface_x];
	r = ((pixel & s->format->Rmask) >> s->format->Rshift);
	g = ((pixel & s->format->Gmask) >> s->format->Gshift);
	b = ((pixel & s->format->Bmask) >> s->format->Bshift);
}

int surface_sheet::get_tile_size() const {
	return tile_size;
}

}
