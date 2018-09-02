#ifndef DEFINES_HPP
#define DEFINES_HPP

struct SDL_Texture;

#define PI 3.141592653589793

# if SDL_BYTEORDER == SDL_BIG_ENDIAN

const unsigned rmask = 0xff000000;
const unsigned gmask = 0x00ff0000;
const unsigned bmask = 0x0000ff00;
const unsigned amask = 0x000000ff;

const unsigned COLONIAL = 0xFAFADCFF; // light square color
const unsigned SEPIA = 0x784614FF; // dark square color

# else

const unsigned rmask = 0x000000ff;
const unsigned gmask = 0x0000ff00;
const unsigned bmask = 0x00ff0000;
const unsigned amask = 0xff000000;

const unsigned COLONIAL = 0xFFDCFAFF; // light square color
const unsigned SEPIA = 0xFF144678;// dark square color

# endif

#define ABS(x) (x < 0 ? -x : x)
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

//const int BOARD_WIDTH = 9;
//const int BOARD_HEIGHT = 7;

const int TEX_WIDTH = 60;
const int TEX_HEIGHT = 60;
//const unsigned ARR_STRIFE = 10; // amount of pixels used from the arrow texture for stretching

const int DEFAULT_WIDTH = 9; // classical: 9
const int DEFAULT_HEIGHT = 7; // classical: 7
const int DEFAULT_PAWNS = 10; // classical: 10
const int DEFAULT_KNIGHTS = 6; // classical: 6
const int DEFAULT_FLANKING = 2; // classical: 2

const double PAWN_VALUE = 1.0;
const double KNIGHT_VALUE = 3.0;

const double PAWN_RESERVE_VALUE = 1.0;
const double KNIGHT_RESERVE_VALUE = 3.0;

const double PAWN_CAPTURE_VALUE = 0.8;
const double KNIGHT_CAPTURE_VALUE = 2.5;

const double CENTER_POSITION_VALUE = 0.1;
const double MOVE_VALUE = 0.01;

const double BBOX_SIZE = 0.9;

struct Rect {
		double x, y;
		double w, h;
};

struct Move {
		int x1, y1;
		int x2, y2;
};

struct Turn {
		char move_count;
		char flags;
		Move moves[3];
};

#define TURN_MOVE 0x01
#define TURN_REINFORCE 0x02
#define TURN_REINFORCE_KNIGHT 0x04

extern SDL_Texture* tex_pieces;
extern SDL_Texture* tex_selections;

#endif // DEFINES_HPP
