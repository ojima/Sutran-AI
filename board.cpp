#include <algorithm>
#include <cmath>
#include <cstring>
#include <random>
#include <sstream>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "board.hpp"
#include "defines.hpp"
#include "piece.hpp"
#include "utils.hpp"

Board::Board(int width, int height) {
	this->width = width;
	this->height = height;
	this->selection = nullptr;
	this->turn = true;
	this->passstate = 0;

	this->reinforcing = false;
	this->reinforce_knight = false;

	this->reinstate = 0;

	this->p1_pawns = 0;
	this->p1_knights = 0;
	this->p2_pawns = 0;
	this->p2_knights = 0;

	this->p1_pawns_c = 0;
	this->p1_knights_c = 0;
	this->p2_pawns_c = 0;
	this->p2_knights_c = 0;

	this->piece_moves = new Piece*[3];
	this->moves = new Move[3];
	this->position_history = "";

	for (int i = 0; i < 3; i++) {
		piece_moves[i] = nullptr;
	}
}

Board::Board(Board* b, Turn t) {
	this->width = b->width;
	this->height = b->height;
	this->selection = nullptr;
	this->turn = b->turn;
	this->passstate = b->passstate;

	this->reinforcing = (t.flags & TURN_REINFORCE);
	this->reinforce_knight = (t.flags & TURN_REINFORCE_KNIGHT);

	this->reinstate = 0;

	this->p1_pawns = b->p1_pawns;
	this->p1_knights = b->p1_knights;
	this->p2_pawns = b->p2_pawns;
	this->p2_knights = b->p2_knights;

	this->p1_pawns_c = b->p1_pawns_c;
	this->p1_knights_c = b->p1_knights_c;
	this->p2_pawns_c = b->p2_pawns_c;
	this->p2_knights_c = b->p2_knights_c;

	this->piece_moves = new Piece*[3];
	this->moves = t.moves;
	this->position_history = std::string(b->position_history);

	for (Piece* p : b->pieces) {
		Piece* q = new Piece(*p);
		pieces.push_back(q);
	}

	for (int i = 0; i < 3; i++) {
		piece_moves[i] = nullptr;
		if (moves[i].x1 != moves[i].x2 and moves[i].y1 != moves[i].y2) {
			piece_moves[i] = this->pieceAt(moves[i].x1, moves[i].y1);
		}
	}

	ChangeTurn();
}

Board::~Board() {
	Clear();
}

void Board::NewGame(int pawns, int knights, int flanking) {
	Clear();

	for (int x = flanking; x < width - flanking; x++) {
		Piece* p = new Piece(this, false, false, x, 0);
		Piece* q = new Piece(this, true, false, x, height - 1);

		pieces.push_back(p);
		pieces.push_back(q);
	}

	for (int x = 0; x < flanking; x++) {
		Piece* k = new Piece(this, false, true, x, 0);
		Piece* l = new Piece(this, false, true, width - x - 1, 0);

		Piece* m = new Piece(this, true, true, x, height - 1);
		Piece* n = new Piece(this, true, true, width - x - 1, height - 1);

		pieces.push_back(k);
		pieces.push_back(l);
		pieces.push_back(m);
		pieces.push_back(n);
	}

	int ps = pawns - width + 2 * flanking;
	int ks = knights - 2 * flanking;

	this->p1_pawns = (ps < 0 ? 0 : ps);
	this->p1_knights = (ks < 0 ? 0 : ks);
	this->p2_pawns = (ps < 0 ? 0 : ps);
	this->p2_knights = (ks < 0 ? 0 : ks);

	this->p1_pawns_c = 0;
	this->p1_knights_c = 0;
	this->p2_pawns_c = 0;
	this->p2_knights_c = 0;

	this->passstate = 2; // Do not permit first-turn passing.
	this->position_history = "";
}

void Board::LoadGame(std::string filename) {
	std::FILE* pFile = std::fopen(filename.c_str(), "r");

	std::fclose(pFile);
}

void Board::SaveGame(std::string filename) {
	std::FILE* pFile = std::fopen(filename.c_str(), "w");
	std::fprintf(pFile, position_history.c_str());
	std::fclose(pFile);
}

void Board::Clear() {
	while (!pieces.empty()) {
		Piece* p = pieces.back();
		pieces.pop_back();
		delete p;
	}
}

bool Board::isEmpty(int x, int y) {
	for (Piece* p : this->pieces) {
		if (p->getX() == x and p->getY() == y) {
			return false;
		}
	}

	return true;
}

Piece* Board::pieceAt(int x, int y) {
	for (Piece* p : this->pieces) {
		if (p->getX() == x and p->getY() == y) {
			return p;
		}
	}

	return nullptr;
}

void Board::ChangeTurn() {
	bool didmove = false;
	if (reinforcing) {
		Piece* p = new Piece(this, turn, reinforce_knight, moves[0].x2, moves[0].y2);
		pieces.push_back(p);

		if (turn) {
			if (reinforce_knight) p1_knights--;
			else p1_pawns--;
		} else {
			if (reinforce_knight) p2_knights--;
			else p2_pawns--;
		}
	} else {
		for (int i = 0; i < 3; i++) {
			Piece* p = piece_moves[i];

			if (p == nullptr) {
				continue;
			}

			Move m = moves[i];

			Piece* q = pieceAt(m.x2, m.y2);
			if (q != nullptr) {
				if (turn) {
					if (p->isKnight()) p1_knights++;
					else p1_pawns++;

					if (q->isKnight()) p1_knights_c++;
					else p1_pawns_c++;
				} else {
					if (p->isKnight()) p2_knights++;
					else p2_pawns++;

					if (q->isKnight()) p2_knights_c++;
					else p2_pawns_c++;
				}

				RemovePiece(q);
				RemovePiece(p);
			} else {
				p->setX(m.x2);
				p->setY(m.y2);
			}
			piece_moves[i] = nullptr;
			didmove = true;
		}
	}

	if (didmove) {
		passstate = 0;
	} else {
		if (passstate == 2) return;
		passstate++;
	}

	reinforcing = false;
	reinforce_knight = false;
	selection = nullptr;
	turn = !turn;
	reinstate = 0;

	this->position_history += summary() + '\n';
}

std::vector<Turn> Board::possibleTurns() {
	std::vector<Turn> turns;

	// Pass
	if (passstate < 2) {
		Turn t;
		t.move_count = 0;
		t.flags = TURN_MOVE;
		turns.push_back(std::move(t));
	}

	// Captures
	for (unsigned i = 0; i < pieces.size(); i++) {
		if (pieces[i]->getSide() != turn) continue;

		std::vector<Move> pmoves = pieces[i]->getLegalMoves(true);

		for (Move m : pmoves) {
			Piece* q = pieceAt(m.x2, m.y2);
			if (q == nullptr) continue;
			if (q->getSide() == turn) continue;
			if (!q->canCapture()) continue;

			Turn t;
			t.move_count = 1;
			t.moves[0] = m;
			t.flags = TURN_MOVE;
			turns.push_back(std::move(t));
		}
	}

	// Reinforcements
	for (int i = 0; i < width; i++) {
		if (turn) {
			if (!isEmpty(i, height - 1)) continue;

			if (p1_knights > 0) {
				Turn t;
				t.move_count = 1;
				t.moves[0] = {-1, -1, i, height - 1};
				t.flags = TURN_REINFORCE | TURN_REINFORCE_KNIGHT;
				turns.push_back(std::move(t));
			}
			if (p1_pawns > 0) {
				Turn t;
				t.move_count = 1;
				t.moves[0] = {-1, -1, i, height - 1};
				t.flags = TURN_REINFORCE;
				turns.push_back(std::move(t));
			}
		} else {
			if (!isEmpty(i, 0)) continue;

			if (p2_knights > 0) {
				Turn t;
				t.move_count = 1;
				t.moves[0] = {-1, -1, i, 0};
				t.flags = TURN_REINFORCE | TURN_REINFORCE_KNIGHT;
				turns.push_back(std::move(t));
			}
			if (p2_pawns > 0) {
				Turn t;
				t.move_count = 1;
				t.moves[0] = {-1, -1, i, 0};
				t.flags = TURN_REINFORCE;
				turns.push_back(std::move(t));
			}
		}
	}

	// Moves
	for (unsigned i = 0; i < pieces.size(); i++) {
		if (pieces[i]->getSide() != turn) continue;

		std::vector<Move> pmoves = pieces[i]->getLegalMoves(false);

		for (unsigned j = 0; j < i; j++) {
			if (pieces[j]->getSide() != turn) continue;

			std::vector<Move> qmoves = pieces[j]->getLegalMoves(false);

			for (unsigned k = 0; k < j; k++) {
				if (pieces[k]->getSide() != turn) continue;

				std::vector<Move> rmoves = pieces[k]->getLegalMoves(false);

				for (Move m : pmoves) {
					for (Move n : qmoves) {
						if (m.x2 == n.x2 and m.y2 == n.y2) continue;

						for (Move l : rmoves) {
							if (m.x2 == l.x2 and m.y2 == l.y2) continue;
							if (n.x2 == l.x2 and n.y2 == l.y2) continue;

							Turn t;
							t.move_count = 3;
							t.moves[0] = m;
							t.moves[1] = n;
							t.moves[2] = l;
							t.flags = TURN_MOVE;
							turns.push_back(t);
						}
					}
				}
			}

			for (Move m : pmoves) {
				for (Move n : qmoves) {
					if (m.x2 == n.x2 and m.y2 == n.y2) continue;

					Turn t;
					t.move_count = 2;
					t.moves[0] = m;
					t.moves[1] = n;
					t.flags = TURN_MOVE;
					turns.push_back(t);
				}
			}
		}

		for (Move m : pmoves) {
			Turn t;
			t.move_count = 1;
			t.moves[0] = m;
			t.flags = TURN_MOVE;
			turns.push_back(t);
		}
	}

	return turns;
}

double AlphaBetaPrune(Board* board, int depth, double alpha, double beta, std::vector<std::pair<std::size_t, double>>& hashtable) {
	std::vector<Turn> turns = board->possibleTurns();

	if (depth == 0 or turns.size() == 0) return board->Evaluate();

	double val, wal;
	Board* b;

	if (board->getTurn()) {
		val = -1000.0;

		for (unsigned j = 0; j < turns.size(); j++) {
			b = new Board(board, turns[j]);
			std::size_t hash = b->hash();
			bool found = false;

			for (std::pair<std::size_t, double> pp : hashtable) {
				if (pp.first == hash) {
					found = true;
					wal = pp.second;
					break;
				}
			}

			if (!found) {
				wal = AlphaBetaPrune(b, depth - 1, alpha, beta, hashtable);
				hashtable.push_back(std::make_pair(hash, wal));
			}
			delete b;

			if (wal > val) val = wal;
			if (val > alpha) alpha = val;
			if (alpha >= beta) break;
		}
	} else {
		val = +1000.0;

		for (unsigned j = 0; j < turns.size(); j++) {
			b = new Board(board, turns[j]);
			std::size_t hash = b->hash();
			bool found = false;

			for (std::pair<std::size_t, double> pp : hashtable) {
				if (pp.first == hash) {
					found = true;
					wal = pp.second;
					break;
				}
			}

			if (!found) {
				wal = AlphaBetaPrune(b, depth - 1, alpha, beta, hashtable);
				hashtable.push_back(std::make_pair(hash, wal));
			}
			delete b;

			if (wal < val) val = wal;
			if (val < beta) beta = val;
			if (alpha >= beta) break;
		}
	}

	return val;
}

double PrincipalVariationPrune(Board* board, int depth, double alpha, double beta, int color) {
	std::vector<Turn> turns = board->possibleTurns();
	if (depth == 0 or turns.size() == 0) return board->Evaluate() * color;

	Board* b = new Board(board, turns[0]);
	double score = -PrincipalVariationPrune(b, depth - 1, -beta, -alpha, -color);
	delete b;

	for (unsigned i = 1; i < turns.size(); i++) {
		Board* b = new Board(board, turns[i]);
		score = -PrincipalVariationPrune(b, depth - 1, -alpha - 1, -alpha, -color);
		if (alpha < score and score < beta) {
			score = -PrincipalVariationPrune(b, depth - 1, -beta, -score, -color);
		}

		delete b;
		if (score > alpha) alpha = score;
		if (alpha >= beta) break;
	}

	return alpha;
}

void Board::ComputeTurn(int depth = 0) {
	// Minmax algorithm with AB-pruning
	std::vector<Turn> turns = possibleTurns();

	printf("Damn, I can do %lu things!\n", turns.size());
	unsigned start = SDL_GetTicks();

	// Random move picker
	/*
	 static std::random_device rd;
	 static std::mt19937 gen(rd());
	 std::uniform_int_distribution<> rng(0, turns.size() - 1);

	 Turn t = turns[rng(gen)];
	 for (int i = 0; i < t.move_count; i++) {
	 piece_moves[i] = pieceAt(t.moves[i].x1, t.moves[i].y1);
	 this->moves[i] = t.moves[i];
	 }

	 printf("Picked random move!\n");
	 */

	double val, wal;
	unsigned bt = -1;
	Board* b;

	if (depth == 0) depth = (int) std::log10(10000.0 / turns.size());
	if (depth < 1) depth = 1;

	printf("Evaluating moves up to depth %d.\n", depth);

	double alpha = -1000.0;
	double beta = +1000.0;
	std::vector<std::pair<std::size_t, double>> hashtable;
	const unsigned k = (unsigned) 100;

	if (turn) {
		val = -1000.0;

		for (unsigned j = 0; j < turns.size(); j++) {
			b = new Board(this, turns[j]);
			std::size_t hash = b->hash();
			bool found = false;

			for (std::pair<std::size_t, double> pp : hashtable) {
				if (pp.first == hash) {
					found = true;
					wal = pp.second;
					break;
				}
			}

			if (!found) {
				wal = AlphaBetaPrune(b, depth - 1, alpha, beta, hashtable);
				hashtable.push_back(std::make_pair(b->hash(), wal));
			}
			delete b;

			if (j > 0 and j % k == 0) printf("[%u/%lu] %.1f s (%lu hashes)\n", j, turns.size(), ((0.001 * (turns.size() - j) / turns.size()) * (SDL_GetTicks() - start)), hashtable.size());

			if (wal > val) {
				val = wal;
				bt = j;
			}
			if (val > alpha) alpha = val;
			if (alpha >= beta) break;
		}
	} else {
		val = +1000.0;

		for (unsigned j = 0; j < turns.size(); j++) {
			b = new Board(this, turns[j]);
			std::size_t hash = b->hash();
			bool found = false;

			for (std::pair<std::size_t, double> pp : hashtable) {
				if (pp.first == hash) {
					found = true;
					wal = pp.second;
					break;
				}
			}

			if (!found) {
				wal = AlphaBetaPrune(b, depth - 1, alpha, beta, hashtable);
				hashtable.push_back(std::make_pair(b->hash(), wal));
			}
			delete b;

			if (j > 0 and j % k == 0) printf("[%u/%lu] %.1f s (%lu hashes)\n", j, turns.size(), ((0.001 * (turns.size() - j) / turns.size()) * (SDL_GetTicks() - start)), hashtable.size());

			if (wal < val) {
				val = wal;
				bt = j;
			}
			if (val < beta) beta = val;
			if (alpha >= beta) break;
		}
	}

	printf("It took me %.1f seconds to compute my move.\n", (0.001 * (SDL_GetTicks() - start)));

	Turn t = turns[bt];
	for (int i = 0; i < t.move_count; i++) {
		piece_moves[i] = pieceAt(t.moves[i].x1, t.moves[i].y1);
		this->moves[i] = t.moves[i];
	}

	this->reinforcing = (t.flags & TURN_REINFORCE);
	this->reinforce_knight = (t.flags & TURN_REINFORCE_KNIGHT);

	if (t.flags & TURN_REINFORCE) {
		printf("Reinforcements arriving at (%d,%d).\n", t.moves[0].x2, t.moves[0].y2);
	}

	ChangeTurn();
}

int Board::WinState() {
	std::vector<std::pair<std::string, int>> checklist;
	std::stringstream ss(position_history);
	std::string line;
	bool found;

	while (std::getline(ss, line, '\n')) {
		found = false;
		for (std::pair<std::string, int> pp : checklist) {
			if (pp.first == line) {
				found = true;
				if (++pp.second >= 3) {
					return 1; // draw by repetition.
				}

				break;
			}
		}

		if (!found) {
			checklist.push_back(std::make_pair(line, 1));
		}
	}

	// No draw by threefold repetition...
	// checking for pieces < 4 or no pieces on board.
	int white = 0, black = 0;
	for (Piece* p : pieces) {
		if (p->getSide()) white++;
		else black++;
	}

	if (white == 0 or white + p1_pawns + p1_knights < 4) return 3; // black wins
	else if (black == 0 or black + p2_pawns + p2_knights < 4) return 2; // white wins.

	// Final check: see if either side is capable of making moves.
	found = turn;

	turn = true;
	white = 0; // undecided
	if (possibleTurns().size() == 0) white = 3;
	turn = false;
	if (possibleTurns().size() == 0) white = 2;
	turn = found;
	return white;
}

double Board::Evaluate() {
	int state = WinState();

	switch (state) {
		case 1:
			return 0.0;
		case 2:
			return +1000.0;
		case 3:
			return -1000.0;
		default:
			break;
	}

	double score = PAWN_RESERVE_VALUE * (p1_pawns - p2_pawns) + KNIGHT_RESERVE_VALUE * (p1_knights - p2_knights);
	score += PAWN_CAPTURE_VALUE * (p1_pawns_c - p2_pawns_c) + KNIGHT_CAPTURE_VALUE * (p1_knights_c - p2_knights_c);

	for (Piece* p : this->pieces) {
		if (p->getSide()) {
			score += p->Evaluate();
		} else {
			score -= p->Evaluate();
		}
	}

	return score;
}

void Board::RemovePiece(Piece* p) {
	pieces.erase(std::remove(pieces.begin(), pieces.end(), p), pieces.end());
	delete p;
}

void Board::Render(SDL_Renderer* context) {
	for (int i = 0; i < this->width; i++) {
		for (int j = 0; j < this->height; j++) {
			SDL_Rect tgt = { (int) (i * TEX_WIDTH), (int) (j * TEX_HEIGHT), (int) TEX_WIDTH, (int) TEX_HEIGHT };

			if ((i % 2) == (j % 2)) {
				SDL_SetRenderDrawColor(context, 250, 250, 220, 255);
			} else {
				SDL_SetRenderDrawColor(context, 150, 80, 30, 255);
			}

			SDL_RenderFillRect(context, &tgt);
		}
	}

	int mx, my;
	unsigned state = SDL_GetMouseState(&mx, &my);

	mx -= mx % TEX_WIDTH;
	my -= my % TEX_HEIGHT;

	int tx = mx / TEX_WIDTH;
	int ty = my / TEX_HEIGHT;

	static bool lastpress = false;
	bool press = (state & SDL_BUTTON(SDL_BUTTON_LEFT));
	bool right = (state & SDL_BUTTON(SDL_BUTTON_RIGHT));

	if (right) {
		selection = nullptr;
	} else if (press and not lastpress) {
		Piece* p = pieceAt(tx, ty);

		if (selection == nullptr) {
			if (p != nullptr) {
				if (p->getSide() == turn) {
					selection = p;
				}
			} else {
				// empty tile click, attempt to reinforce.
				if (turn and ty == height - 1) {
					if (p1_pawns > 0 and reinstate == 0) {
						reinstate = 1;
						moves[0] = {0, 0, tx, ty};
					}
					else if (p1_knights > 0 and (reinstate == 1 or p1_pawns == 0)) {
						reinstate = 2;
						moves[0] = {0, 0, tx, ty};
					}
					else {
						reinstate = 0;
					}
				} else if (ty == 0) {
					if (p2_pawns > 0 and reinstate == 0) {
						reinstate = 1;
						moves[0] = {0, 0, tx, ty};
					}
					else if (p2_knights > 0 and (reinstate == 1 or p2_pawns == 0)) {
						reinstate = 2;
						moves[0] = {0, 0, tx, ty};
					}
					else {
						reinstate = 0;
					}
				}

				if (reinstate != 0) {
					piece_moves[0] = nullptr;
					piece_moves[1] = nullptr;
					piece_moves[2] = nullptr;
					reinforce_knight = (reinstate == 2);
					selection = nullptr;
					reinforcing = true;
				}
			}
		} else {
			if (p != nullptr) {
				if (selection == p) selection = nullptr;
				else if (p->getSide() == turn) selection = p;
				else if (selection->isLegalMove(tx, ty, true)) {
					// capturing
					moves[0] = {selection->getX(), selection->getY(), tx, ty};
					piece_moves[0] = selection;
					piece_moves[1] = nullptr;
					piece_moves[2] = nullptr;
					selection = nullptr;
					reinforcing = false;
					reinstate = 0;
				}
			} else {
				// Legal move
				if (selection->isLegalMove(tx, ty, true)) {
					for (int i = 0; i < 3; i++) {
						if (piece_moves[i] == nullptr) {
							moves[i] = {selection->getX(), selection->getY(), tx, ty};
							piece_moves[i] = selection;
							reinforcing = false;
							reinstate = 0;
							break;
						} else {
							if (piece_moves[i] == selection) {
								moves[i] = {selection->getX(), selection->getY(), tx, ty};
								reinforcing = false;
								reinstate = 0;
								break;
							} else if (moves[i].x2 == tx and moves[i].y2 == ty) {
								piece_moves[i] = selection;
								moves[i] = {selection->getX(), selection->getY(), tx, ty};
								reinforcing = false;
								reinstate = 0;
								break;
							} else if (!isEmpty(moves[i].x2, moves[i].y2)) {
								piece_moves[i] = selection;
								moves[i] = {selection->getX(), selection->getY(), tx, ty};
								reinforcing = false;
								reinstate = 0;
								break;
							}
						}
					}

					selection = nullptr;
				} else {
					printf("Illegal move!\n");
					selection = nullptr;
				}
			}
		}
	}

	lastpress = press;

	SDL_Rect r, s;
	r.x = mx;
	r.y = my;
	r.w = TEX_WIDTH;
	r.h = TEX_HEIGHT;

	s.x = 0;
	s.y = 0;
	s.w = TEX_WIDTH;
	s.h = TEX_HEIGHT;

	int a = (int) (128.0 + 100.0 * sin(2.0 * PI * SDL_GetTicks() / 5000.0));
	SDL_SetRenderDrawBlendMode(context, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(tex_selections, 255, 255, 255);
	SDL_SetTextureAlphaMod(tex_selections, a);
	SDL_RenderCopy(context, tex_selections, &s, &r);
//SDL_SetRenderDrawColor(context, 255, 255, 255, a);
//SDL_RenderFillRect(context, &r);

	if (selection != nullptr) {
		r.x = selection->getX() * TEX_WIDTH;
		r.y = selection->getY() * TEX_HEIGHT;
		r.w = TEX_WIDTH;
		r.h = TEX_HEIGHT;

		SDL_SetTextureColorMod(tex_selections, 0, 100, 0);
		SDL_SetTextureAlphaMod(tex_selections, 255);
		SDL_RenderCopy(context, tex_selections, &s, &r);

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (i == selection->getX() and j == selection->getY()) continue;

				r.x = i * TEX_WIDTH;
				r.y = j * TEX_HEIGHT;
				r.w = TEX_WIDTH;
				r.h = TEX_HEIGHT;

				if (selection->isLegalMove(i, j, true)) {
					SDL_SetTextureColorMod(tex_selections, 0, 100, 0);
					SDL_SetTextureAlphaMod(tex_selections, 150);
					s = {TEX_WIDTH, 0, TEX_WIDTH, TEX_HEIGHT};
					SDL_RenderCopy(context, tex_selections, &s, &r);
				} else {
					SDL_SetRenderDrawColor(context, 70, 70, 70, 150);
					SDL_RenderFillRect(context, &r);
				}
			}
		}
	}

	if (reinstate != 0) {
		SDL_SetTextureAlphaMod(tex_pieces, 150);
		r.x = moves[0].x2 * TEX_WIDTH;
		r.y = moves[0].y2 * TEX_HEIGHT;
		r.w = TEX_WIDTH;
		r.h = TEX_HEIGHT;
		s = {(reinstate == 2 ? 0 : TEX_WIDTH), (turn ? TEX_HEIGHT : 0), TEX_WIDTH, TEX_HEIGHT};
		SDL_RenderCopy(context, tex_pieces, &s, &r);
	}

	SDL_SetTextureAlphaMod(tex_pieces, 255);
	for (Piece* p : this->pieces) {
		p->Render(context, turn);
	}

	SDL_SetTextureColorMod(tex_selections, 0, 100, 0);
	SDL_SetTextureAlphaMod(tex_selections, 150);
	for (int i = 0; i < 3; i++) {
		if (this->piece_moves[i] != nullptr) {
			Move m = moves[i];

			RenderArrow(context, m.x1 * TEX_WIDTH, m.y1 * TEX_HEIGHT, m.x2 * TEX_WIDTH, m.y2 * TEX_HEIGHT);
		}
	}
}

int Board::renderWidth() {
	return TEX_WIDTH * this->width;
}

int Board::renderHeight() {
	return TEX_HEIGHT * this->height;
}

std::string Board::summary() {
	std::string result = "[";

	int j = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Piece* p = pieceAt(x, y);
			if (p == nullptr) {
				j++;
			} else {
				if (j > 0) {
					result += std::to_string(j);
					j = 0;
				}

				if (p->getSide() and p->isKnight()) result += "K";
				if (p->getSide() and !p->isKnight()) result += "P";
				if (!p->getSide() and p->isKnight()) result += "k";
				if (!p->getSide() and !p->isKnight()) result += "p";
			}
		}

		if (j > 0) result += std::to_string(j);
		if (y < height - 1) result += "/";
		j = 0;
	}

	result += ']';
	result += ' ' + std::to_string(passstate);
	result += ' ' + std::to_string(p1_pawns);
	result += ' ' + std::to_string(p1_knights);
	result += ' ' + std::to_string(p2_pawns);
	result += ' ' + std::to_string(p2_knights);
	result += ' ' + std::to_string((int) turn);

	return result;
}

std::size_t Board::hash() {
	static std::hash<std::string> hash_fn;
	return hash_fn(this->summary());
}
