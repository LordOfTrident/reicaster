#ifndef APP_HH_HEADER_GUARD
#define APP_HH_HEADER_GUARD

#include <cstdlib> // std::size_t
#include <cassert> // assert
#include <vector>  // std::vector
#include <map>     // std::map

#include <SDL2/SDL.h>
#include <chol/log.h>

#include "common.hh"
#include "pixels.hh"
#include "map.hh"
#include "math.hh"
#include "raycast.hh"
#include "entity.hh"
#include "asset.hh"
#include "surface_sheet.hh"

namespace reicaster {

constexpr auto win_w     = 1000,  win_h     = 600;
constexpr auto view_3d_w = win_w, view_3d_h = win_h;

constexpr auto map_w = 20, map_h = 20;

constexpr auto tile_size = 300;

constexpr float fov = 80;
constexpr float sensitivity = 0.4;

constexpr auto  plr_h         = 0.5;
constexpr auto  plr_size      = 0.2;
constexpr float plr_max_speed = 0.04;
constexpr float plr_accel     = 0.01;
constexpr float plr_jump_vel  = 0.1;

constexpr float friction      = 0.88;
constexpr float gravity_accel = 0.01;

constexpr auto plr_vert_off_range = 800;

class application {
public:
	static application *create(const char *title);
	static application &instance();

	application(const char *title);
	~application();

	void render();
	void update();

	bool is_running() const;

private:
	static application *instance_ptr;

	void render_view_3d();
	void render_column(const ray_hit &hit, int x, float dir);

	void handle_events();

	SDL_Window   *win;
	SDL_Renderer *ren;
	SDL_Event     evt;
	const Uint8  *keys;

	map level;

	float  vert_off, bump_off;
	entity plr;

	// Temporary
	vec2f light_src;
	int   strafe;

	std::map<const char*, asset> assets;
	surface_sheet sheet;

	float dist_from_proj_plane;

	pixels       view_3d;
	SDL_Texture *view_3d_texture;

	std::size_t tick;
	bool        running, paused;
};

}

#endif
