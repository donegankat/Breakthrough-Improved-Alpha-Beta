/*
 *  Final Alpha Beta.h
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/10/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */

#ifndef FINAL ALPHA BETA_H
#define FINAL ALPHA BETA_H


#include "Game.h"
#include "Eval.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector

static int GetNodesSearchedLast();
bool P1Win(BoardState s);
bool P2Win(BoardState s);
bool GameOver(BoardState s);
static int GetOppositeTurn(int turn);

static int ModifyDepth(int depth, int PossibleMoves);
std::vector<BoardState> SortBoards(std::vector<BoardState> MyBoards);
static std::vector<BoardState> PossibleBoards(BoardState s, int player);
std::vector<PieceAction> PossibleMoves(BoardState s, int player);

uint64_t MakeNullMove(BoardState board);
uint64_t UnmakeNullMove(BoardState board);

static BoardState FixedDepthAlphaBeta(BoardState ExamineBoard, int MaxDepth, int playerTurn);
PieceAction FixedDepthAlphaBetaMove(BoardState ExamineBoard, int MaxDepth, int playerTurn);

static BoardState AlphaBetaTTRoot(BoardState ExamineBoard, int Depth, int turn, bool AllowNullMove);
PieceAction AlphaBetaTTRootMove(BoardState ExamineBoard, int Depth, int turn, bool AllowNullMove);

static int AlphaBetaTT(BoardState ExamineBoard, int Depth, int Alpha, int Beta, int turn, bool AllowNullMove);

void RunAlphaBeta(BoardState &s, int turn, int depth, int a, int b);

#endif