#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <string>
#include "defines.hpp"

class Piece;

class Board {
	public:
		Board(int width, int height);
		Board(Board* b, Turn t);
		~Board();

		void NewGame(int pawns, int knights, int flanking);
		void Clear();
		void Render(SDL_Renderer* context);
		bool isEmpty(int x, int y);
		Piece* pieceAt(int x, int y);

		void ChangeTurn();

		void ComputeTurn(int depth);
		double Evaluate();
		int WinState();
		std::vector<Turn> possibleTurns();

		void LoadGame(std::string filename);
		void SaveGame(std::string filename);

	protected:
		int width, height, passstate, reinstate;
		std::vector<Piece*> pieces;
		Piece** piece_moves;
		Move* moves;
		Piece* selection;
		bool turn;
		int p1_pawns, p1_knights, p1_pawns_c, p1_knights_c;
		int p2_pawns, p2_knights, p2_pawns_c, p2_knights_c;
		std::string position_history;

		void RemovePiece(Piece* p);

	public:
		inline int getWidth() {
			return this->width;
		}
		inline int getHeight() {
			return this->height;
		}

		inline bool getTurn() {
			return turn;
		}

		int renderWidth();
		int renderHeight();

		std::string summary();
		std::size_t hash();
};

#endif // BOARD_HPP
