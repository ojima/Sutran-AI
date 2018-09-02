# Sutran-AI
Dirty implementation of the board game Sutran in C++ with a "working" AI using AB-pruning to explore the game tree.

# How to build

The files need to be compiled with any C++ compiler and linked against `SDL2` (`SDL2main` on Windows) and `SDL2_image`. An example of a makefile for this would be:

    COMP = g++
    FLAG = -c -Wall -O2
    LINK = -lSDL2 -lSDL2_image
    SRCS = $(wildcard *.cpp)
    OBJS = $(patsubst %.cpp, %.o, %(SRCS))
    EXEC = SutranAI
    
    all : $(SRCS) $(EXEC)
    
    $(EXEC) : $(OBJS)
        $(COMP) $(OBJS) $(LINK) -o $@
    
    .cpp.o :
        $(COMP) $(FLAG) $< -o $@
 
 The program uses SDL2 for rendering and input handling, and standard libraries for the core engine.
 
 # Rules of the game
 
 A game of Sutran is played (usually) on a 9x7 board with two types of units: soldiers and knights. The goal of the game is to either force your opponent to resign, have no enemy units on the board or reduce the total opposing army to less than 4 pieces.
 
 At setup, both players have a row of nine pieces on opposing sides of the board: four knights in groups of two at the sides and five soldiers in the center. Additionally, each player has five pawns and two knights in their reserves. Each turn, a player may:
 
 - pass their turn, giving the turn back to their opponent,
 - reinforce their army by taking a single piece from their reinforcements and placing it on an empty tile in their home row,
 - capture an opponent with one of their own pieces, or
 - move up to three pieces according to the rules.

Soldiers may move one tile horizontally or vertically, knights may move two tiles horizontally or vertically or any combination of those. If two opposing units stand right besides each other (horizontally/vertically), they are said to be *skirmishing* and may not move sideways with respect to each other, only backwards. This also means that a knight cannot move past an opposing unit in one turn, since moving adjecent to that unit means it cannot move further.

In order to capture an opposing unit, a player first needs to ensure that it is bordered (on all _eight_ sides) by more units from their side than of the opposing side (this includes the unit about to be captured). This means for example, that two same-side soldiers standing on adjecent tiles can only be captured if they are bordered by at least _three_ opposing units. When this condition is fulfilled, the capturing player must be able to move any piece onto the tile occupied by the targeting piece. This move would then be their only legal move that turn, and the player will have to take both their piece and the opponent piece of the board, but only their own piece goes back to reserves and can later return to the game.

# Controls

The controls of Sutran are reasonably simple - I think most if not all of the difficulty would come from it's lack of user-friendliness.

Left-click on a piece to select it, right-click anywhere to deselect it. Selecting a piece will highlight its legal moves, and clicking on any of the highlighted tiles will show an arrow pointing to the position where you want to move your piece. Left-click on an empty tile in the home row will cycle through reinforcement options.
Pressing `B` or `Numpad-5` will confirm a set of moves (making no moves will count as passing your turn), execute it and change turns to the other side.
Pressing `D` or `Numpad-8` will make the computer evaluate all possible moves and make the best one it can find. By default, the computer will evaluate 4 moves deep, but this can be changed with the `+` and `-` keys. I recommend leaving it at 4 or lowering it to 3 if you find that your PC takes too long to compute moves. In my experience, leaving it at 4 takes about 10 seconds to find a good move, although some moves can suddenly spike up to a minute or more.
Pressing `N` or `Numpad-1` will start a new game, and pressing `C` or `Numpad-0` will clear the board (which is pointless because I haven't implemented a "scenario editor" function yet).
Pressing `S` will export the current game state to a file called `sutran.txt` in a FEN-esque format.
Finally, pressing `Q` or `Numpad-9` will quit the game (although this may not work while your computer is evaluating a new move).

# Tweaking the game

If you find the board too large, want more reserves, or want to tweak the AI's evaluation values, you can easily modify these values in `defines.hpp`. The values `DEFAULT_WIDTH` or `DEFAULT_HEIGHT` refer to the dimensions of the board, `DEFAULT_PAWNS` and `DEFAULT_KNIGHTS` refers to the amount of soldiers and knights available to each player at the start of the game (both on board and in reserves), and `DEFAULT_FLANKING` refers to the amount of knights present in the corners of the board at the start.

The values below that are used in the evaluation function of the AI. The AI values a game state based on the value of its pieces (1 point for soldiers, 3 for knights), how many pieces it has captured (0.8 for soldiers, 2.5 for knights), their position on the board (+0.1 points for each tile they moved away from the edge of the board) and how many possible moves each piece can make (+0.01 points for each tile). Tweaking these values a bit (or a lot) may cause the AI to make different moves.

# Legal notes

The board game of Sutran and its name are property of ojima, the author.
You are free to use this program and its source code, provided you give credit where credit is due.
