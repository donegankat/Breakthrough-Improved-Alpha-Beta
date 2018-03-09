/*
 *  Game.h
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/9/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */


#ifndef GAME_H
#define GAME_H

#include <ostream>
using std::ostream;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <stdint.h>

#include "Move.h"
#include "Piece.h"
//#include "Alpha Beta.h"


inline size_t row_major(size_t x, size_t y, size_t row_size)
{
    return x * row_size + y;
}

struct BoardState {
	int board[64];
	int playerTurn;
	int boardValue;
	//	PieceAction LastAction;
	
	//	BoardState()
	//	{
	
	//	}
	
};
struct PieceAction
{
	int from, to, depth, player;
	
	PieceAction()
	{
		from = 0;
		to = 0;
	}
	
	PieceAction(int f, int t)
	{
		from = f;
		to = t;
	}
	
	PieceAction(int f, int t, int d, int p)
	{
		from = f;
		to = t;
		depth = d;
		player = p;
	}
};

BoardState NullBoard();
BoardState GetStartState();
uint64_t GetHashFromState(BoardState s);
BoardState GetStateFromHash(uint64_t hash);
uint64_t GetMaxHashValue();
void BruteForceHashValidate();
int GetLineNum(int a);
int GetColumnNum(int a);
bool MoveForward(BoardState s, int index, int turn);
bool MoveDiagonalRight(BoardState s, int index, int turn);
bool MoveDiagonalLeft(BoardState s, int index, int turn);
bool isCaptureRight(BoardState s, int index, int turn);
bool isCaptureLeft(BoardState s, int index, int turn);
vector<PieceAction> GetLegalActions1(BoardState s);
vector<PieceAction> GetLegalActions2(BoardState s);
vector<PieceAction> GetPieceActions(BoardState s, int turn, int index);
int GetNumActions1(BoardState s);
int GetNumActions2(BoardState s);

BoardState ApplyAction(BoardState s, PieceAction a);
uint64_t ZobristApplyAction(uint64_t hash, int turn, PieceAction a);

BoardState UndoAction(BoardState s, PieceAction a);
uint64_t ZobristUndoAction(uint64_t hash, int turn, PieceAction a);

bool isLegalAction(BoardState s, int fromIndex, int toIndex);
int GetIndex(int x, int y);
int GetColumn(int index);
int GetRow(int index);
void PrintBoard(BoardState currentBoard);
int GetGameOver(BoardState s);
void SwitchTurn(BoardState s);
void Play(BoardState s);

BoardState GetBoardState(BoardState s);
int BasicEval(BoardState s, int index);
int EvalPiece(BoardState s, int index);
bool GetWin(BoardState s, int index);
bool GetLoss(BoardState s, int index);
int GetNumPieces1(BoardState s);
int GetNumPieces2(BoardState s);
//static int GetNum1();
//static int GetNum2();

void HumanTurn(BoardState s, int currentDepth);
int ConvertChar(char x);

void AB1(BoardState s, int d);
void AB2(BoardState s, int d);
void testAB(std::vector<PieceAction> x, BoardState s);

class Game
{
public:
	
	
	
	
	
private:
    const size_t board_size;
    vector<BoardState> board;
    vector<Piece> pieces;
    vector<PieceAction> move_history;
	//    Players turn;
	
};


#endif
