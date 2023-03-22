#include "app.hh"

namespace reicaster {

application *application::instance_ptr = nullptr;

application *application::create(const char *title) {
	instance_ptr = new application(title);
	return instance_ptr;
}

application &application::instance() {
	if (instance_ptr == nullptr)
		assert(0 && "instance_ptr is nullptr");

	return *instance_ptr;
}

application::application(const char *title):
	plr(plr_size, plr_h, plr_max_speed),
	strafe(0),
	dist_from_proj_plane(view_3d_w / 2 / std::tan(dtor(fov / 2))),
	tick(0),
	running(true),
	paused(false)
{
	log_set_flags(LOG_TIME | LOG_LOC);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		LOG_FATAL("Failed to initialize SDL2: %s", SDL_GetError());
	else
		LOG_INFO("Initialized SDL");

	win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                       win_w, win_h, SDL_WINDOW_SHOWN);
	if (win == nullptr)
		LOG_FATAL("Failed to create a window: %s", SDL_GetError());
	else
		LOG_INFO("Created a window");

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == nullptr)
		LOG_FATAL("Failed to create a renderer: %s", SDL_GetError());
	else
		LOG_INFO("Created a renderer");

	if (SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND) < 0)
		LOG_FATAL("Failed to set render draw blend mode: %s", SDL_GetError());
	else
		LOG_INFO("Render draw blend mode was set");

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"))
		LOG_FATAL("Failed to set SDL_HINT_RENDER_SCALE_QUALITY hint: %s", SDL_GetError());
	else
		LOG_INFO("SDL_HINT_RENDER_SCALE_QUALITY hint was set to 'linear'");

	view_3d.create(view_3d_w, view_3d_h);
	LOG_INFO("Created the 3d view pixels");

	view_3d_texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888,
	                                    SDL_TEXTUREACCESS_STREAMING, view_3d_w, view_3d_h);
	if (view_3d_texture == nullptr)
		LOG_FATAL("Failed to create the 3d view texture: %s", SDL_GetError());
	else
		LOG_INFO("Created the 3d view texture");

	keys = SDL_GetKeyboardState(nullptr);

	if (SDL_ShowCursor(SDL_DISABLE) < 0)
		LOG_FATAL("Failed to hide the cursor: %s", SDL_GetError());
	else
		LOG_INFO("Hid the cursor");

	// TODO: System for reading levels from a file
	level.create(map_w, map_h,
	             "####################"
	             "#        #         #"
	             "# #    # #         #"
	             "W   #    #         #"
	             "# #      #         #"
	             "# #  W   _         #"
	             "# ##     #   #-#   #"
	             "W #    # #         #"
	             "#        W         #"
	             "# W#_#_###         #"
	             "#                  #"
	             "#W##  #  #  #  #  ##"
	             "#                  #"
	             "#                  #"
	             "#                  #"
	             "#        #W        #"
	             "#                  #"
	             "#                  #"
	             "#  ##  ##  ##  ##  #"
	             "####################");
	LOG_INFO("Created the level");

	assets["sheet"]  = asset::create_from(ren, "res/sheet.bmp", false);
	LOG_INFO("Loaded assets");

	sheet.create(assets["sheet"].s, tile_size);

	plr.pos.x = 3.5;
	plr.pos.y = 3.5;
	plr.dir   = 45;
	vert_off  = 0;

	light_src.x = 3.5;
	light_src.y = 5.5;

	LOG_INFO("Finished initialization");
	LOG_INFO("------------------------");
}

application::~application() {
	for (auto &[_, a] : assets)
		a.destroy();
	LOG_INFO("Destroyed the assets");

	level.destroy();
	LOG_INFO("Destroyed the level");

	view_3d.destroy();
	LOG_INFO("Destroyed the 3d view pixels");

	SDL_DestroyTexture(view_3d_texture);
	LOG_INFO("Destroyed the 3d view texture");

	SDL_DestroyRenderer(ren);
	LOG_INFO("Destroyed the renderer");

	SDL_DestroyWindow(win);
	LOG_INFO("Destroyed the window");

	SDL_Quit();
	LOG_INFO("Quit SDL2");
}

void application::render() {
	SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(ren);

	render_view_3d();

	if (paused) {
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 128);
		SDL_RenderFillRect(ren, nullptr);
	}

	SDL_RenderPresent(ren);
}

void application::render_column(const ray_hit &hit, int x, float dir) {
	(void)dir;

	if (hit.at->h == 0)
		return;

	if (not paused)
		bump_off = std::sin(static_cast<float>(tick) / 4) *
		           (plr.vel.mag() * view_3d_h) * (1 / hit.dist);

	int h = std::round(1.0 / hit.dist * win_h);
	vec2i pos(x, win_h / 2 - h / 2 - vert_off + bump_off);
	pos.y -= h / 2;
	pos.y += (plr.z + plr.h) * h;

	pos.y -= h * hit.at->z;
	pos.y += h * (1 - hit.at->h);
	h     *= hit.at->h;

	if (pos.y >= static_cast<int>(view_3d.h) or
	    pos.x >= static_cast<int>(view_3d.w) or h <= 0)
		return;

	int off = 0;
	if (pos.y < 0)
		off = -pos.y;

	float darken_by = 1 / (1 + distance(hit.pos,
	                                    light_src + std::sin(static_cast<float>(tick) / 10)));
	float column;
	bool  invert = false;
	switch (hit.side) {
	case direction::left: invert = true;
		// fall through
	case direction::right:
		column = hit.pos.y - std::floor(hit.pos.y);
		break;

	case direction::down: invert = true;
		// fall through
	case direction::up:
		column     = hit.pos.x - std::floor(hit.pos.x);
		darken_by *= 0.8;
		break;

	default: assert(0 && "Unknown direction value");
	}

	auto  ratio = static_cast<float>(sheet.get_tile_size()) / h * hit.at->h;
	vec2i surface_pos(column * sheet.get_tile_size(), 0);

	if (invert)
		surface_pos.x = sheet.get_tile_size() - surface_pos.x - 1;

	for (int y = off; y < h; ++ y) {
		if (static_cast<std::size_t>(pos.y + y) >= view_3d.h)
			break;

		surface_pos.y = static_cast<float>(y) * ratio;

		int r, g, b;
		sheet.get_color_at(hit.at->id(hit.side), surface_pos.x, surface_pos.y, r, g, b);
		r *= darken_by;
		g *= darken_by;
		b *= darken_by;

		if (r < 0)
			r = 0;
		if (g < 0)
			g = 0;
		if (b < 0)
			b = 0;

		view_3d.at(pos.x, pos.y + y) = color_to_pixel_abgr(r, g, b);
	}
}

void application::render_view_3d() {
	view_3d.clear();
	for (int x = 0; x < view_3d_w; ++ x) {
		float dir = plr.dir + rtod(std::atan2(static_cast<float>(x) - view_3d_w / 2,
		                                      dist_from_proj_plane));

		// TODO: Optimize this rendering, so the pixels arent overwritten a bunch of times for no
		//       reason:
		//           - Loop from start to end, not end to start
		//           - Detect the start and end of the column that was rendered, and only render
		//             where the columns havent been drawn yet
		auto hits = cast_ray(level, plr.pos, vec2f::from_deg(dir));
		for (int i = hits.size() - 1; i >= 0; -- i) {
			if (hits[i].out_of_bounds)
				continue;

			hits[i].dist *= std::cos(dtor(plr.dir - dir));
			render_column(hits[i], x, dir);
		}
	}

	SDL_UpdateTexture(view_3d_texture, nullptr, view_3d.raw, view_3d.w * 4);
	SDL_RenderCopyEx(ren, view_3d_texture, nullptr, nullptr, strafe, nullptr, SDL_FLIP_NONE);
}

void application::update() {
	++ tick;
	handle_events();

	float h = (1 + std::sin(static_cast<float>(tick) / 4)) / 2;
	level.at(9,  15).h = h;
	level.at(10, 15).h = 1 - h;

	if (not paused) {
		plr.vel_z -= gravity_accel;
		plr.update(level, friction);
	}
}

void application::handle_events() {
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
		case SDL_KEYDOWN:
			switch (evt.key.keysym.sym) {
			case SDLK_ESCAPE: paused    = not paused;   break;
			case SDLK_SPACE:  plr.vel_z = plr_jump_vel; break;

			default: break;
			}
			break;

		case SDL_QUIT: running = false; break;
		case SDL_MOUSEMOTION:
			if (paused)
				break;

			SDL_WarpMouseInWindow(win, win_w / 2, win_h / 2);

			plr.dir  += static_cast<float>(evt.motion.x - win_w / 2) / 50 * sensitivity;
			vert_off += static_cast<float>(evt.motion.y - win_h / 2) / 10 * sensitivity;

			if (vert_off > plr_vert_off_range / 2)
				vert_off = plr_vert_off_range / 2;
			else if (vert_off < -plr_vert_off_range / 2)
				vert_off = -plr_vert_off_range / 2;

			break;

		default: break;
		}
	}

	if (not paused) {
		strafe = 0;
		if (keys[SDL_SCANCODE_W])
			plr.accelerate(plr_accel);
		if (keys[SDL_SCANCODE_A]) {
			plr.accelerate(-plr_accel, plr.dir + 90);
			strafe = 1;
		}
		if (keys[SDL_SCANCODE_S])
			plr.accelerate(-plr_accel);
		if (keys[SDL_SCANCODE_D]) {
			plr.accelerate(plr_accel, plr.dir + 90);
			strafe = -1;
		}
	}
}

bool application::is_running() const {
	return running;
}

}
