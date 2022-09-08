#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"
#include "Load.hpp"
#include "LoadedData.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

// basic snake game knowledge followed from https://github.com/pknowledge/C-Snake-Game/blob/master/main.cpphttps://github.com/pknowledge/C-Snake-Game/blob/master/main.cpp
void PlayMode::gen_food() {
    food.x = ((rand() % ((ppu.ScreenWidth / 8) - 3)) + 1) * 8;
    food.y = ((rand() % ((ppu.ScreenHeight / 8) - 3)) + 1) * 8;
}

bool PlayMode::food_eaten() {
	return (abs(snake.x - food.x) < 8 && abs(snake.y - food.y) < 8);
}

void PlayMode::snake_grow() {
	snake.len++;
}

LoadedData pipeline;
Load<LoadedData::TileData> snake_left(LoadTagDefault, []() -> LoadedData::TileData * {return pipeline.load_sprite("assets/snake_head_left.png");});
Load<LoadedData::TileData> snake_right(LoadTagDefault, []() -> LoadedData::TileData * {return pipeline.load_sprite("assets/snake_head_right.png");});
Load<LoadedData::TileData> snake_up(LoadTagDefault, []() -> LoadedData::TileData * {return pipeline.load_sprite("assets/snake_head_up.png");});
Load<LoadedData::TileData> snake_down(LoadTagDefault, []() -> LoadedData::TileData * {return pipeline.load_sprite("assets/snake_head_down.png");});
Load<LoadedData::TileData> snake_body(LoadTagDefault, []() -> LoadedData::TileData * {return pipeline.load_sprite("assets/snake_body.png");});
Load<LoadedData::TileData> food_pixels(LoadTagDefault, []() -> LoadedData::TileData * {return pipeline.load_sprite("assets/food.png");});
Load<LoadedData::TileData> light_background(LoadTagDefault, []() -> LoadedData::TileData * {return pipeline.load_sprite("assets/light_background.png");});


PlayMode::PlayMode() {
	// init assets in starting positions
	gen_food();
	snake.body.push_back(std::make_pair(snake.x, snake.y));

	
	// TILES
	//--background
	const LoadedData::TileData light_background_tile = *light_background;
	ppu.tile_table[0].bit0 = light_background_tile.tile.bit0;
	ppu.tile_table[0].bit1 = light_background_tile.tile.bit1;

	//---snake head
	// left
	const LoadedData::TileData snake_left_tile = *snake_left;
	ppu.tile_table[5].bit0 = snake_left_tile.tile.bit0;
	ppu.tile_table[5].bit1 = snake_left_tile.tile.bit1;
	// right
	const LoadedData::TileData snake_right_tile = *snake_right;
	ppu.tile_table[6].bit0 = snake_right_tile.tile.bit0;
	ppu.tile_table[6].bit1 = snake_right_tile.tile.bit1;
	// up
	const LoadedData::TileData snake_up_tile = *snake_up;
	ppu.tile_table[7].bit0 = snake_up_tile.tile.bit0;
	ppu.tile_table[7].bit1 = snake_up_tile.tile.bit1;
	// down
	const LoadedData::TileData snake_down_tile = *snake_down;
	ppu.tile_table[8].bit0 = snake_down_tile.tile.bit0;
	ppu.tile_table[8].bit1 = snake_down_tile.tile.bit1;
	
	//---snake body
	const LoadedData::TileData snake_body_tile = *snake_body;
	ppu.tile_table[9].bit0 = snake_body_tile.tile.bit0;
	ppu.tile_table[9].bit1 = snake_body_tile.tile.bit1;

	//---food
	const LoadedData::TileData food_tile = *food_pixels;
	ppu.tile_table[10].bit0 = food_tile.tile.bit0;
	ppu.tile_table[10].bit1 = food_tile.tile.bit1;


	// PALETTES
	//---background
	ppu.palette_table[0] = light_background_tile.palette;
	//---snake
	ppu.palette_table[1] = snake_body_tile.palette;
	//---food
	ppu.palette_table[2] = food_tile.palette;


}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		left.pressed = false;
		right.pressed = false;
		up.pressed = false;
		down.pressed = false;
		if (evt.key.keysym.sym == SDLK_LEFT) {
			snake.dir = 0;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			snake.dir = 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			snake.dir = 2;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			snake.dir = 3;
			down.pressed = true;
			return true;
		}
	}
	return false;
}

void PlayMode::tick() {
	constexpr float PlayerSpeed = 8.0f;
	if (left.pressed) snake.x -= PlayerSpeed;
	if (right.pressed) snake.x += PlayerSpeed;
	if (down.pressed) snake.y -= PlayerSpeed;
	if (up.pressed) snake.y += PlayerSpeed;

	snake.body.push_back(std::make_pair((snake.x / 8) * 8, (snake.y / 8) * 8));
	if (snake.body.size() > snake.len) {
		snake.body.pop_front();
	}

	if (food_eaten()) {
		snake_grow();
		gen_food();
	}
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):

	tick_acc += elapsed;
	while (tick_acc > Tick) {
		tick_acc -= Tick;
		tick();
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	// background
	for (uint32_t i = 0; i < ppu.BackgroundWidth * ppu.BackgroundHeight; i++) {
		ppu.background[i] = 0;
	}
	
	// food sprite:
	ppu.sprites[0].x = food.x;
	ppu.sprites[0].y = food.y;
	ppu.sprites[0].index = 10;
	ppu.sprites[0].attributes = 2;

	int idx = 2;
	for (std::deque< std::pair<uint8_t,uint8_t> >::iterator it = snake.body.begin(); it != snake.body.end() - 1; it++) {
		// snake body sprites:
		ppu.sprites[idx].x = it->first;
		ppu.sprites[idx].y = it->second;
		ppu.sprites[idx].index = 9;
		ppu.sprites[idx].attributes = 1;
		idx++;
	}

	// snake head sprite:
	ppu.sprites[1].x = snake.body.back().first;
	ppu.sprites[1].y = snake.body.back().second;
	ppu.sprites[1].index = snake.dir + 5;
	ppu.sprites[1].attributes = 1;


	//--- actually draw ---
	ppu.draw(drawable_size);
}
