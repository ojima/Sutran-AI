#ifndef PIECE_HPP
#define PIECE_HPP

#include <vector>

class Board;
struct Move;

class Piece {
	public:
		Piece(Piece& p);
		Piece(Board* board, bool color, bool knight, int x, int y);

		bool canCapture();

		~Piece();

		bool isLegalMove(int nx, int ny, bool mayCapture);
		std::vector<Move> getLegalMoves(bool mayCapture);
		void Render(SDL_Renderer* context, bool turn);
		double Evaluate();

	protected:
		Board* board;
		bool color, knight;
		int x, y;

	public:
		Rect* getBounds();

		inline int getX() {
			return x;
		}

		inline int getY() {
			return y;
		}

		inline void setX(int x) {
			this->x = x;
		}

		inline void setY(int y) {
			this->y = y;
		}

		inline bool getSide() {
			return this->color;
		}

		inline bool isKnight() {
			return this->knight;
		}
};

#endif // PIECE_HPP
