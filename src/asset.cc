#include "asset.hh"

namespace reicaster {

asset asset::create_from(SDL_Renderer *r, Uint8 *bytes, std::size_t size,
                         bool create_texture) {
	asset a;
	a.load(r, bytes, size, create_texture);
	return a;
}

asset asset::create_from(SDL_Renderer *r, const char *path, bool create_texture) {
	asset a;
	a.load(r, path, create_texture);
	return a;
}

void asset::load(SDL_Renderer *r, Uint8 *bytes, std::size_t size, bool create_texture) {
	auto rw = SDL_RWFromMem(static_cast<void*>(bytes), static_cast<int>(size));
	if (rw == nullptr)
		LOG_FATAL("Failed to create rw for byte array %p: %s",
		          static_cast<void*>(bytes), SDL_GetError());

	s = SDL_LoadBMP_RW(rw, 1);
	if (s == nullptr)
		LOG_FATAL("Failed to load bmp byte array %p: %s",
		          static_cast<void*>(bytes), SDL_GetError());

	if (create_texture) {
		t = SDL_CreateTextureFromSurface(r, s);
		if (t == nullptr)
			LOG_FATAL("Failed to create texture from bmp byte array %p: %s",
			          static_cast<void*>(bytes), SDL_GetError());
	} else
		t = nullptr;
}

void asset::load(SDL_Renderer *r, const char *path, bool create_texture) {
	s = SDL_LoadBMP(path);
	if (s == nullptr)
		LOG_FATAL("Failed to load bmp file %s: %s", path, SDL_GetError());

	if (create_texture) {
		t = SDL_CreateTextureFromSurface(r, s);
		if (t == nullptr)
			LOG_FATAL("Failed to create texture from bmp file %s: %s", path, SDL_GetError());
	} else
		t = nullptr;
}

void asset::destroy() {
	if (s != nullptr)
		SDL_FreeSurface(s);
	if (t != nullptr)
		SDL_DestroyTexture(t);
}

}
