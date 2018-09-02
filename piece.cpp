#include <SDL2/SDL.h>
#include <utility>

#include "defines.hpp"
#include "board.hpp"
#include "piece.hpp"

Piece::Piece(Piece& p) {
	this->board = p.board;
	this->color = p.color;
	this->knight = p.knight;
	this->x = p.x;
	this->y = p.y;
}

Piece::Piece(Board* board, bool color, bool knight, int x, int y) {
	this->board = board;
	this->color = color;
	this->knight = knight;
	this->x = x;
	this->y = y;
}

Piece::~Piece() {
}

bool Piece::isLegalMove(int nx, int ny, bool mayCapture) {
	if (nx < 0 or ny < 0) return false;
	if (nx >= board->getWidth() or ny >= board->getHeight()) return false;
	if (nx == x and ny == y) return false;
	if (board->pieceAt(nx, ny) != nullptr and not mayCapture) return false;

	int dx = nx - x;
	int dy = ny - y;

	if (ABS(dx) + ABS(dy) > (knight ? 2 : 1)) {
		return false;
	}

	// Check "skirmish rule" (cannot move past enemy piece, only away)
	Piece* p = nullptr;
	if (knight) {
		// Check "skirmish rule" (cannot move past enemy piece, only away)
		// Moving two tiles straight? Check whether passing is possible (not blocked & not passing by enemy)
		if (dx == +2) {
			if (not board->isEmpty(x + 1, y)) return false;

			p = board->pieceAt(x + 1, y + 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
			p = board->pieceAt(x + 1, y - 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
		} else if (dx == -2) {
			if (not board->isEmpty(x - 1, y)) return false;

			p = board->pieceAt(x - 1, y + 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
			p = board->pieceAt(x - 1, y - 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
		} else if (dy == +2) {
			if (not board->isEmpty(x, y + 1)) return false;

			p = board->pieceAt(x + 1, y + 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
			p = board->pieceAt(x - 1, y + 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
		} else if (dy == -2) {
			if (not board->isEmpty(x, y - 1)) return false;

			p = board->pieceAt(x + 1, y - 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
			p = board->pieceAt(x - 1, y - 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
		}

		if (dx != 0 and dy == 0) {
			p = board->pieceAt(x, y + 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
			p = board->pieceAt(x, y - 1);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
		} else if (dy != 0 and dx == 0) {
			p = board->pieceAt(x + 1, y);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
			p = board->pieceAt(x - 1, y);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
		} else if (dx != 0 and dy != 0) {
			p = board->pieceAt(x + dx, y);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
			p = board->pieceAt(x, y + dy);
			if (p != nullptr) {
				if (p->getSide() != color) return false;
			}
		}
	}

	if (dx == +1 or dx == -1) {
		p = board->pieceAt(x, y + 1);
		if (p != nullptr) {
			if (p->getSide() != color) return false;
		}
		p = board->pieceAt(x, y - 1);
		if (p != nullptr) {
			if (p->getSide() != color) return false;
		}
	} else if (dy == +1 or dy == -1) {
		p = board->pieceAt(x + 1, y);
		if (p != nullptr) {
			if (p->getSide() != color) return false;
		}
		p = board->pieceAt(x - 1, y);
		if (p != nullptr) {
			if (p->getSide() != color) return false;
		}
	}

	if ((p = board->pieceAt(nx, ny)) != nullptr) {
		if (!mayCapture) return false;
		if (p->getSide() == color) return false;
		if (!p->canCapture()) return false;
	}

	return true;
}

bool Piece::canCapture() {
	int allies = 0, enemies = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			Piece* p = board->pieceAt(x + i, y + j);
			if (p == nullptr) continue;

			if (p->getSide() == color) allies++;
			else enemies++;
		}
	}

	return (enemies > allies);
}

std::vector<Move> Piece::getLegalMoves(bool mayCapture) {
	std::vector<Move> results;

	for (int i = -2; i <= 2; i++) {
		for (int j = -2; j <= 2; j++) {
			if (isLegalMove(x + i, y + j, mayCapture)) {
				results.push_back( { x, y, x + i, y + j });
			}
		}
	}

	return results;
}

void Piece::Render(SDL_Renderer* context, bool turn) {
	int tx = (knight ? 0 : TEX_WIDTH);
	int ty = (color ? TEX_HEIGHT : 0);
	int tw = TEX_WIDTH;
	int th = TEX_HEIGHT;

	SDL_Rect src = { tx, ty, tw, th };
	SDL_Rect dst = { (int) x * tw, (int) y * th, tw, th };

	SDL_RenderCopy(context, tex_pieces, &src, &dst);

	if (turn != color and canCapture()) {
		SDL_SetTextureColorMod(tex_selections, 255, 0, 0);
		SDL_SetTextureAlphaMod(tex_selections, 150);
		src = {0, 0, TEX_WIDTH, TEX_HEIGHT};
		SDL_RenderCopy(context, tex_selections, &src, &dst);
	}
}

double Piece::Evaluate() {
	double base = (knight ? KNIGHT_VALUE : PAWN_VALUE);
	double move = MOVE_VALUE * getLegalMoves(true).size();
	double post = CENTER_POSITION_VALUE * (MIN(x, board->getWidth() - x) + MIN(y, board->getHeight() - y));
	return base + move + post;
}

Rect* Piece::getBounds() {
	Rect* r = new Rect;
	r->x = TEX_WIDTH * (x + (1.0 - BBOX_SIZE) / 2.0);
	r->y = TEX_HEIGHT * (y + (1.0 - BBOX_SIZE) / 2.0);
	r->w = BBOX_SIZE * TEX_WIDTH;
	r->h = BBOX_SIZE * TEX_HEIGHT;
	return r;
}
