#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "defines.hpp"

SDL_Texture* tex_pieces = nullptr;
SDL_Texture* tex_selections = nullptr;

bool LoadTextures(SDL_Renderer* context) {
	SDL_Surface* load = IMG_Load("figures.png");

	if (load == nullptr) {
		return false;
	}

	tex_pieces = SDL_CreateTextureFromSurface(context, load);
	SDL_FreeSurface(load);

	if (tex_pieces == nullptr) {
		return false;
	}

	load = IMG_Load("selections.png");

	if (load == nullptr) {
		return false;
	}

	tex_selections = SDL_CreateTextureFromSurface(context, load);
	SDL_FreeSurface(load);

	if (tex_selections == nullptr) {
		return false;
	}

	return true;
}

void RenderArrow(SDL_Renderer* context, int x1, int y1, int x2, int y2) {
	int len = (int) std::sqrt(std::pow(std::abs(x2 - x1) + TEX_WIDTH, 2.0) + std::pow(y2 - y1, 2.0));
	double ang = std::atan2(y2 - y1, x2 - x1) * 180.0 / PI;

	SDL_Rect src = { 0, TEX_HEIGHT, 2 * TEX_WIDTH, TEX_HEIGHT }, dst = { x1, y1, len, TEX_HEIGHT };
	SDL_Point pt = { TEX_WIDTH / 2, TEX_HEIGHT / 2 };

	SDL_RenderCopyEx(context, tex_selections, &src, &dst, ang, &pt, SDL_FLIP_NONE);
}
