#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//Fixed-time update helper (called by 'update'):
	void tick();
	static constexpr float Tick = 0.1f; //timestep used for tick()
	float tick_acc = 0.0f; //accumulated time toward next tick
	uint8_t tick_count = 0; //count ticks for longer events

	//----- game state -----
	bool isGameOver = false;
	bool isGameWon = false;


	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	// Food
	struct Food {
		uint8_t x;
		uint8_t y;
	} food;
	
	void gen_food();


	// Snake
	struct Snake {
		uint8_t x = PPU466::ScreenWidth / 2;
		uint8_t y = PPU466::ScreenHeight / 2;
		uint8_t dir = 2; // 0=Left, 1=Right, 2=Up, 3=Down
		uint8_t len = 1;
		std::deque< std::pair<uint8_t,uint8_t> > body;
	} snake;
	bool food_eaten();
	void snake_grow();
	void snake_shrink();


	std::array<uint32_t, 20> game_over_idxs = {20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400};

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
