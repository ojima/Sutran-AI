#include <SDL2/SDL.h>
#include <cstdio>

#include "board.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::printf("Failed to initialize SDL: %s.\n", SDL_GetError());
		return 1;
	}

	std::printf("Initialized SDL.\n");

	Board board(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	board.NewGame(DEFAULT_PAWNS, DEFAULT_KNIGHTS, DEFAULT_FLANKING);

	SDL_Window* window = SDL_CreateWindow("SutranAI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, board.renderWidth(), board.renderHeight(), SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::printf("Failed to create SDL Window: %s.\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer* context = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	std::printf("Created SDL Window.\n");

	if (!LoadTextures(context)) {
		std::printf("Failed to load textures: %s.\n", SDL_GetError());
	}

	bool running = true;
	int depth = -6;
	SDL_Event e;
	while (running) {
		bool didTick = false;

		while (SDL_PollEvent(&e)) {
			didTick = true;
			if (e.type == SDL_QUIT) {
				printf("Quitting!\n");
				running = false;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_n:
					case SDLK_KP_1:
						printf("New game!\n");
						board.NewGame(DEFAULT_PAWNS, DEFAULT_KNIGHTS, DEFAULT_FLANKING);
						break;

					case SDLK_c:
					case SDLK_KP_0:
						printf("Clearing board!\n");
						board.Clear();
						break;

					case SDLK_q:
					case SDLK_KP_9:
						printf("Quitting!\n");
						running = false;
						break;

					case SDLK_b:
					case SDLK_KP_5:
						board.ChangeTurn();
						printf("Changing turns!\nCurrent evaluation: %.1f\n", board.Evaluate());
						printf("%s\n", board.summary().c_str());
						break;

					case SDLK_d:
					case SDLK_KP_8:
						board.ComputeTurn(depth);
						printf("Current evaluation: %.1f\n", board.Evaluate());
						printf("%s\n", board.summary().c_str());
						break;

					case SDLK_PLUS:
					case SDLK_EQUALS:
					case SDLK_KP_PLUS:
						if (depth < 1000) depth++;
						printf("Increasing evaluation depth to %d.\n", depth);
						break;

					case SDLK_MINUS:
					case SDLK_KP_MINUS:
						depth--;
						printf("Decreasing evaluation depth to %d.\n", depth);
						break;

					case SDLK_s:
						printf("Saving game.\n");
						board.SaveGame("sutran.txt");
						break;

					default:
						break;
				}
			}
		}

		if (not didTick) {
			SDL_Delay(1);
		}

		SDL_RenderClear(context);
		board.Render(context);
		SDL_RenderPresent(context);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
