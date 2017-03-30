#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct Game game_t;
typedef struct Pacman pacman_t;
typedef struct Ghost ghost_t;
typedef struct Coin coin_t;
typedef struct Powerup powerup_t;

struct Pacman {
	bool alive;
	int32_t x, y;
	uint8_t lives;
	double powerup_time;
};

struct Ghost {
	bool alive;
	int32_t x, y;
};

struct Coin {
	bool taken;
	int32_t x, y;
};

struct Powerup {
	bool taken;
	int32_t x, y;
};

struct Game {
	uint32_t pts;
	pacman_t pacman;
	ghost_t ghosts[4];
	coin_t coins[128];
	powerup_t powerups[4];
};


game_t start_game()
{
	uint32_t width = 12, height = 12;

	game_t game;
	game.pts = 0;
	game.pacman = (pacman_t) { .alive = true, .x = 0, .y = 0, .lives = 3, .powerup_time = 0 };

	game.ghosts[0] = (ghost_t) { .alive = true, .x = -width/2, .y = -height/2 };
	game.ghosts[1] = (ghost_t) { .alive = true, .x = width/2, .y = -height/2 };
	game.ghosts[2] = (ghost_t) { .alive = true, .x = width/2, .y = height/2 };
	game.ghosts[3] = (ghost_t) { .alive = true, .x = -width/2, .y = height/2 };

	game.powerups[0] = (powerup_t) { .taken = false, .x = width, .y = -height };
	game.powerups[1] = (powerup_t) { .taken = false, .x = width, .y = height };
	game.powerups[2] = (powerup_t) { .taken = false, .x = -width, .y = height };
	game.powerups[3] = (powerup_t) { .taken = false, .x = - width, .y = -height };

	for (uint32_t i = 0; i < 128; i++) {
		int32_t x = i % width;
		int32_t y = i / width;
		game.coins[i] = (coin_t) { .taken = false, .x = x, .y = y };
	}

	return game;
}

typedef struct key_sample_t key_sample_t;

struct key_sample_t {
	bool w, a, s, d;
	bool q;
};

/* Get keypress from virtual keyboard */
key_sample_t sample_input()
{
	key_sample_t sample;

	sample.w = true;
	sample.a = false;
	sample.s = false;
	sample.d = false;

	sample.q = false;

	return sample;
};

/* Move pacman in 2d space */
void steer_pacman(pacman_t *pacman, key_sample_t *input)
{
	if (input->w)
		pacman->y = pacman->y - 1;
	if (input->s)
		pacman->y = pacman->y + 1;
	if (input->a)
		pacman->x = pacman->x - 1;
	if (input->d)
		pacman->x = pacman->x + 1;
}

void pickup_coins(game_t *game)
{
	for (uint32_t i = 0; i < 128; i++) {
		coin_t *coin = &game->coins[i];

		if (coin->taken) continue;
		if (coin->x != game->pacman.x) continue;
		if (coin->y != game->pacman.y) continue;

		coin->taken = true;
		game->pts = game->pts + 1;
	}
}

void pickup_powerups(game_t *game)
{
	for (uint32_t i = 0; i < 4; i++) {
		powerup_t *powerup = &game->powerups[i];

		if (powerup->taken) continue;
		if (powerup->x != game->pacman.x) continue;
		if (powerup->y != game->pacman.y) continue;

		powerup->taken = true;
		game->pacman.powerup_time += 30;
	}
}

void steer_ghosts(ghost_t ghosts[4])
{
	for (uint32_t i = 0; i < 4; i++) {
		ghost_t *ghost = &ghosts[i];

		if (!ghost->alive) continue;
		int32_t x = i - 2;
		int32_t y = x < 0 ? -1 : 1;

		ghost->x = ghost->x + x;
		ghost->y = ghost->y + y;
	}
}

void collide_ghosts(game_t *game) {
	for (uint32_t i = 0; i < 4; i++) {
		ghost_t *ghost = &game->ghosts[i];

		if (!ghost->alive) continue;
		if (ghost->x != game->pacman.x) continue;
		if (ghost->y != game->pacman.y) continue;

		if (game->pacman.powerup_time > 0) {
			game->pts = game->pts + 30;
			ghost->alive = false;
		} else {
			game->pacman.lives = game->pacman.lives - 1;
			game->pacman.x = 0;
			game->pacman.y = 0;
			game->pacman.alive = game->pacman.lives > 0;
		}
	}
}

void run_game(game_t *game)
{
	double acc = 0;
	uint64_t runs = 0;

	while (game->pacman.alive) {
		clock_t start = clock();

		for (uint32_t k = 0; k < 1000; k++) {
			key_sample_t input = sample_input();

			steer_pacman(&game->pacman, &input);
			steer_ghosts(game->ghosts);
			pickup_coins(game);
			pickup_powerups(game);
			collide_ghosts(game);
		}

		clock_t finish = clock();
		double time = (finish-start) / (double)CLOCKS_PER_SEC;

		if (game->pacman.powerup_time > 0)
			game->pacman.powerup_time -= time;

		acc += time;
		runs += 1;

		printf("%f\n", acc/runs);
	}
}

void end_game(game_t *game)
{
	printf("You got %" PRId32 " points!\n", game->pts);
}

void init() {}
void cleanup() {}

int main(int argc, char** argv)
{

	init();

	while (1) {
		game_t game = start_game();
		run_game(&game);
		end_game(&game);
	}

	cleanup();
}
