#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <ostream>
using std::ostream;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <algorithm>

#include "Move.h"
#include "Piece.h"
#include "Players.h"

typedef enum {board_closed, board_open, board_player1, board_player2} Board;

inline size_t row_major(size_t x, size_t y, size_t row_size)
{
    return x * row_size + y;
}

class GameState
{
 public:
    friend ostream& operator<<(ostream& os, GameState& s);

    explicit GameState(size_t board_size_ = 8);
	 // In theory 4-26 supported, but only 8 has been tested

	int get_num_pieces_total() const;
	int get_num_pieces1() const;
	int get_num_pieces2() const;
	
	void set_turn(Players new_player);
	void switch_turn();
	
    vector<Move> get_moves() const; // for current player's move
    vector<Move> get_moves(const Players player) const;
    GameState& apply_move(const Move m); // return self-reference
    GameState& undo_move();
    /*
    int get_winner() const;
    */

	int Evaluate(GameState &s);
	int Evaluate(GameState &s, Players turn);
	
	//Move GetMove();
	
    bool game_over() const;
	Move get_last_move() const;
    Players get_whose_turn() const;
    string pretty_print_location(const size_t location) const;
    string pretty_print_move(const Move m) const;
    void reset();
    Move translate_to_local(const vector<string> message) const;
    bool valid_move(const Move m) const;
    bool valid_move(const Move m, const Players player) const;

	void Run(GameState &s, int MaxDepth, Players turn);
	Move RunMove(GameState &s, int MaxDepth, Players turn);
	
	int ModifyDepth(int depth, int PossibleMoves);
	std::vector<GameState> PossibleBoards(GameState &s, Players turn);
	void FixedDepthAlphaBetaVoid(GameState &s, int MaxDepth, Players turn);
	GameState& FixedDepthAlphaBeta(GameState &s, int MaxDepth, Players turn);
	Move FixedDepthAlphaBetaMove(GameState &s, int MaxDepth, Players turn);
	bool SortSuccessors(GameState i, GameState j);
	void AlphaBetaTTRootVoid(GameState &s, int Depth, Players turn, bool AllowNullMove);
	GameState& AlphaBetaTTRoot(GameState &s, int Depth, Players turn, bool AllowNullMove);
	Move AlphaBetaTTRootMove(GameState &s, int Depth, Players turn, bool AllowNullMove);
	int AlphaBetaTT(GameState &ExamineBoard, int Depth, int Alpha, int Beta, Players turn, bool AllowNullMove);
	
 private:
    size_t board_size;
    vector<Board> board;
    vector<Piece> pieces;
    vector<Move> move_history;
    Players turn;
	//Move my_move;
};

#endif
