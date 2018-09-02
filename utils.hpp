#ifndef UTILS_HPP
#define UTILS_HPP

struct SDL_Renderer;

bool LoadTextures(SDL_Renderer* context);

void RenderArrow(SDL_Renderer* context, int x1, int y1, int x2, int y2);

#endif // UTILS_HPP
