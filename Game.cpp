/*
 *  Game.cpp
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/9/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */

#include "Game.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <ostream>
using std::ostream;
#include <sstream>
using std::ostringstream;
#include <stdint.h>

#include "Eval.h"
#include "ZobristValues.h"
#include "String.h"

static int alpha = Evaluation(GetStartState()); //Minimum
static int beta = 1000000; //Maximum
static PieceAction best_move, worst_move;
//static int depth = 0;
const int maxDepth = 3;
static int num1 = 16;
static int num2 = 16;
static bool LastMoveCapture = false;

static std::vector<PieceAction> moves_taken1, moves_taken2;
static std::vector<PieceAction> best_sequence, worst_sequence;

void AB1(BoardState s, int d);
void AB2(BoardState s, int d);
void testAB(std::vector<PieceAction> x, BoardState s);

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
std::vector<PieceAction> GetLegalActions1(BoardState s);
std::vector<PieceAction> GetLegalActions2(BoardState s);
std::vector<PieceAction> GetPieceActions(BoardState s, int turn, int index);
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
void SwitchTurn(BoardState s);
void Play(BoardState s);

BoardState GetBoardState(BoardState s);
int BasicEval(BoardState s, int index);
int EvalPiece(BoardState s, int index);
bool GetWin(BoardState s, int index);
bool GetLoss(BoardState s, int index);
int GetNumPieces1(BoardState s);
int GetNumPieces2(BoardState s);

void HumanTurn(BoardState s, int currentDepth);
int ConvertChar(char x);

std::vector<PieceAction> move_history;


BoardState NullBoard()
{
	BoardState s;
	
	for(int i = 0; i < 64; i++)
	{
		s.board[i] = 0;
	}
	
	return s;
}

void HumanTurn(BoardState s, int currentDepth)
{
	
	char x, toX;
	int y, toY;
	int index, toIndex;
	int score = 0;
	
	score = Evaluation(s);
	std::cout << "Score = " << score << std::endl;
	
	std::cout << "Player " << s.playerTurn << std::endl;
	std::cout << "Enter the coordinates of the piece you want to move:" << std::endl;
	std::cin >> x >> y;
	
	int xLoc = ConvertChar(x);
	index = GetIndex(xLoc, y - 1);
	
	std::cout << "Index: " << index << std::endl;
	
	if(s.board[index] != s.playerTurn)
	{
		std::cout << "You don't have a piece there." << std::endl;
		return;
	}
	
	std::vector<PieceAction> moves = GetPieceActions(s, s.playerTurn, index);
	
	if(moves.size() == 0)
	{
		std::cout << "That piece has no legal moves." << std::endl;
		return;
	}
	
	std::cout << "The piece at: " << x << " " << y << " can move to: " << std::endl;
	
	for (int i = 0; i < moves.size(); i++)//std::vector<PieceAction>::size_type i = 0; i < moves.size(); ++i)
	{
		//char moveFromColumn = 'A' + GetColumn(moves[i].from);
		//int moveFromRow = GetRow(moves[i].from);
		char moveToColumn = 'A' + GetColumn(moves[i].to);
		int moveToRow = GetRow(moves[i].to) + 1;
		std::cout << moveToColumn << " " << moveToRow << std::endl;
	}
	
	std::cout << std::endl;
	
	std::cout << "Enter the coordinates you want to move to:" << std::endl;
	std::cin >> toX >> toY;
	
	int toXLoc = ConvertChar(toX);
	toIndex = GetIndex(toXLoc, toY - 1);
	int legalMoveNum = -1;
	
	for(int i = 0; i < moves.size(); i++)
	{
		if(moves[i].to == toIndex)
		{
			legalMoveNum = i;
			break;
		}
	}
	
	if(legalMoveNum == -1)
	{
		std::cout << "Illegal action." << std::endl;
		return;
	}
	
	s = ApplyAction(s, moves[legalMoveNum]);
	PrintBoard(s);
	
	SwitchTurn(s);
	
	
	
	
	
	/*
	 
	 s.playerTurn = 2;
	 char x, toX;
	 int y, toY;
	 int index, toIndex;
	 
	 std::cout << "Player " << s.playerTurn << std::endl;
	 std::cout << "Enter the coordinates of the piece you want to move:" << std::endl;
	 std::cin >> x >> y;
	 
	 int xLoc = ConvertChar(x);
	 index = GetIndex(xLoc, y - 1);
	 
	 std::cout << "Index: " << index << std::endl;
	 
	 if(s.board[index] != s.playerTurn)
	 {
	 std::cout << "You don't have a piece there." << std::endl;
	 return;
	 }
	 
	 std::vector<PieceAction> moves = GetPieceActions(s, s.playerTurn, index);
	 
	 if(moves.size() == 0)
	 {
	 std::cout << "That piece has no legal moves." << std::endl;
	 return;
	 }
	 
	 std::cout << "The piece at: " << x << " " << y << " can move to: " << std::endl;
	 
	 for (int i = 0; i < moves.size(); i++)
	 {
	 char moveToColumn = 'A' + GetColumn(moves[i].to);
	 int moveToRow = GetRow(moves[i].to);
	 std::cout << moveToColumn << " " << moveToRow << std::endl;
	 }
	 
	 std::cout << std::endl;
	 
	 std::cout << "Enter the coordinates you want to move to:" << std::endl;
	 std::cin >> toX >> toY;
	 
	 int toXLoc = ConvertChar(toX);
	 toIndex = GetIndex(toXLoc, toY - 1);
	 int legalMoveNum = -1;
	 
	 for(int i = 0; i < moves.size(); i++)
	 {
	 if(moves[i].to == toIndex)
	 {
	 legalMoveNum = i;
	 break;
	 }
	 }
	 
	 if(legalMoveNum == -1)
	 {
	 std::cout << "Illegal action." << std::endl;
	 return;
	 }
	 
	 s = ApplyAction(s, moves[legalMoveNum]);
	 PrintBoard(s);
	 */
}

int EvalPiece(BoardState s, int index)
{
	int score = 0;
	
	for(int i = 0; i < 64; i++)
	{
		if(s.board[i] == 0)
		{
			continue;
		}
		
		if(s.board[i] == 1 && s.playerTurn == 2)
		{
			continue;
		}
		
		if(s.board[i] == 2 && s.playerTurn == 1)
		{
			continue;
		}
		
		if(GetWin(s, i))
		{
			score += 1000;
		}
		
		if(GetLoss(s, i))
		{
			score -= 1000;
		}
		
		//	score += GetLineNum(index);
		score += GetColumnNum(index);
		//		if(score > max)
		//		{
		//			max = score;
		//		}
	}
	
	return score;
}

bool GetWin(BoardState s, int index)
{
	if(MoveForward(s, index, s.playerTurn))
	{
		if(s.playerTurn == 1 && index - 8 >= 0 && index - 8 < 8)
		{
			return 1;
		}
		
		//		if(s.playerTurn == 2 && index + 8 >= 56 && index + 8 < 64)
		//		{
		//			return 2;
		//		}
	}
	
	if(MoveDiagonalRight(s, index, s.playerTurn))
	{
		if(s.playerTurn == 1 && index - 7 >= 0 && index - 7 < 8)
		{
			return true;
		}
		
		//		if(s.playerTurn == 2 && index + 7 >= 56 && index + 7 < 64)
		//		{
		//			return 2;
		//		}
	}
	
	if(MoveDiagonalLeft(s, index, s.playerTurn))
	{
		if(s.playerTurn == 1 && index - 9 >= 0 && index - 9 < 8)
		{
			return true;
		}
		
		//		if(s.playerTurn == 2 && index + 9 >= 56 && index + 9 < 64)
		//		{
		//			return 2;
		//		}
	}
	
	return 0;
}

bool GetLoss(BoardState s, int index)
{
	if(MoveForward(s, index, s.playerTurn))
	{
		if(s.playerTurn == 2 && index + 8 >= 56 && index + 8 < 64)
		{
			return true;
		}
	}
	
	if(MoveDiagonalRight(s, index, s.playerTurn))
	{
		
		if(s.playerTurn == 2 && index + 7 >= 56 && index + 7 < 64)
		{
			return true;
		}
	}
	
	if(MoveDiagonalLeft(s, index, s.playerTurn))
	{
		
		if(s.playerTurn == 2 && index + 9 >= 56 && index + 9 < 64)
		{
			return true;
		}
	}
	
	return false;
	
}

int GetGameOver(BoardState s)
{
	//if(s.playerTurn == 1)
	//{
	for(int i = 0; i < 8; i++)
	{
		if(s.board[i] == 1)
		{
			return 1;
		}
	}
	//}
	
	//else {
	for(int i = 56; i < 64; i++)
	{
		if(s.board[i] == 2)
		{
			return 2;
		}
	}
	//}
	
	return 0;
}

BoardState GetStartState()
{
	BoardState StartState;
	StartState.playerTurn = 1;
	//	StartState.LastAction = NULL;
	
	for(int i = 0; i < 64; i++)
	{
		StartState.board[i] = 0;
	}
	
	for(int i = 0; i < 16; i++)
	{
		StartState.board[i] = 2;
	}
	
	for(int i = 48; i < 64; i++)
	{
		StartState.board[i] = 1;
	}
	
	StartState.boardValue = Evaluation(StartState);
	
	return StartState;
}

BoardState GetBoardState(BoardState s)
{
	BoardState theState;
	
	for(int i = 0; i < 64; i++)
	{
		theState.board[i] = s.board[i];
	}
	
	return theState;
}

uint64_t GetHashFromState(BoardState s)
{
	uint64_t myHash = 0;
	int currentBit = 0;
	
	for (int i = 0; i < 64; i++)
	{
		if (s.board[i] == 1)
		{
			myHash ^= zobrist_P1[i];
		}
		else if (s.board[i] == 2)
		{
			myHash ^= zobrist_P2[i];
		}
		currentBit++;
	}
	
	if(s.playerTurn == 1)
	{
		myHash ^= zobrist_Turn[0];
	}
	else {
		myHash ^= zobrist_Turn[1];
	}
	
	return myHash;
}

BoardState GetStateFromHash(uint64_t hash)
{
	BoardState currentState;
	
	int bit_pos = 0;
	for(int i = 0; i < 64; i++)
	{
		if ((hash>>bit_pos)&1)
		{
			currentState.board[i] = 1;
		}
		else {
			currentState.board[i] = 0;
		}
		bit_pos++;
	}
	
	for(int i = 64; i < 128; i++)
	{
		if ((hash>>bit_pos)&1)
		{
			currentState.board[i-64] = 2;
		}
		bit_pos++;
	}
	
	return currentState;
}

uint64_t GetMaxHashValue()
{
	//5x6 board
	//return 0x3FFFFFFF;
	
	//7x8 board
	//return 0xFFFFFFFF;
}

void BruteForceHashValidate()
{
	for (uint64_t h = 0; h < GetMaxHashValue(); ++h)
		if (h != GetHashFromState(GetStateFromHash(h)))
			std::cout << "Problem with hash " << h << std::endl;
}

int GetLineNum(int a)
{
	
	return a / 8;
	
}

int GetColumnNum(int a)
{
	
	return (a % 8);
	
}

bool MoveForward(BoardState s, int index, int turn)
{
	if(turn == 1 && index - 8 >= 0 && s.board[index] == 1 && s.board[index - 8] == 0 && GetColumnNum(index) == GetColumnNum(index - 8))
	{
		return true;
	}
	
	if(turn == 2 && index + 8 < 64 && GetColumnNum(index) == GetColumnNum(index + 8) && s.board[index] == 2 && s.board[index + 8] == 0)
	{
		return true;
	}
	
	return false;
}

bool MoveDiagonalRight(BoardState s, int index, int turn)
{
	if(turn == 1 && index - 7 >= 0 && s.board[index] == 1 && s.board[index - 7] != 1 && GetColumnNum(index) == GetColumnNum(index - 7) - 1 && GetLineNum(index - 7) >= 0  && GetLineNum(index) > GetLineNum(index - 7))
	{
		return true;
	}
	
	if(turn == 2 && index + 7 < 64  && s.board[index] == 2 && s.board[index + 7] != 2 && GetColumnNum(index) == GetColumnNum(index + 7) + 1 && GetLineNum(index + 7) < 8  && GetLineNum(index) < GetLineNum(index + 7))
	{
		return true;
	}
	
	return false;
}

bool MoveDiagonalLeft(BoardState s, int index, int turn)
{
	
	if(turn == 1 && index - 9 >= 0 && s.board[index] == 1 && s.board[index - 9] != 1 && GetColumnNum(index) == GetColumnNum(index - 9) + 1 && GetLineNum(index) > GetLineNum(index - 9))
	{
		return true;
	}
	
	if(turn == 2 && index + 9 < 64 && s.board[index] == 2 && s.board[index + 9] != 2 && GetColumnNum(index) == GetColumnNum(index + 9) - 1 && GetLineNum(index) < GetLineNum(index + 9))
	{
		return true;
	}
	
	return false;
}

bool isCaptureRight(BoardState s, int index, int turn)
{
	if(turn == 1 && s.board[index - 7] == 2)
	{
		return true;
	}
	
	if(turn == 2 && s.board[index + 7] == 1)
	{
		return true;
	}
	
	return false;
}

bool isCaptureLeft(BoardState s, int index, int turn)
{
	if(turn == 1 && s.board[index - 9] == 2)
	{
		return true;
	}
	
	if(turn == 2 && s.board[index + 9] == 1)
	{
		return true;
	}
	
	return false;
}

std::vector<PieceAction> GetLegalActions1(BoardState s)
{
	std::vector<PieceAction> legalMoves;
	PieceAction forward, diagRight, diagLeft;
	
	for(int index = 0; index < 64; index++)
	{
		if(s.board[index] != 1)
		{
			continue;
		}
		
		if(MoveDiagonalLeft(s, index, 1))
		{
			diagLeft = PieceAction(index, index - 9);
			legalMoves.push_back(diagLeft);
			//			std::cout << "Move: " << index << " to: " << index - 9 << std::endl;
			
		}
		
		if(MoveForward(s, index, 1))
		{
			forward = PieceAction(index, index - 8);
			legalMoves.push_back(forward);
			//			std::cout << "Move: " << index << " to: " << index - 8 << std::endl;
			
		}
		
		if(MoveDiagonalRight(s, index, 1))
		{
			diagRight = PieceAction(index, index - 7);
			legalMoves.push_back(diagRight);
			//			std::cout << "Move: " << index << " to: " << index - 7 << std::endl;
			
		}
	}
	
	return legalMoves;
}

std::vector<PieceAction> GetLegalActions2(BoardState s)
{
	std::vector<PieceAction> legalMoves;
	PieceAction forward, diagRight, diagLeft;
	
	for(int index = 0; index < 64; index++)
	{
		if(s.board[index] != 2)
		{
			continue;
		}
		
		if(MoveDiagonalRight(s, index, 2))
		{
			diagRight = PieceAction(index, index + 7);
			legalMoves.push_back(diagRight);
			//			std::cout << "Move: " << index << " to: " << index + 7 << std::endl;
		}		
		
		if(MoveForward(s, index, 2))
		{
			forward = PieceAction(index, index + 8);
			legalMoves.push_back(forward);
			//			std::cout << "Move: " << index << " to: " << index + 8 << std::endl;
		}
		
		if(MoveDiagonalLeft(s, index, 2))
		{
			diagLeft = PieceAction(index, index + 9);
			legalMoves.push_back(diagLeft);
			//			std::cout << "Move: " << index << " to: " << index + 9 << std::endl;
		}	
	}
	
	return legalMoves;
}

std::vector<PieceAction> GetPieceActions(BoardState s, int turn, int index)
{
	std::vector<PieceAction> legalMoves;
	PieceAction forward, diagRight, diagLeft;
	
	if(turn == 1)//s.playerTurn == 1)
	{
		if(MoveDiagonalRight(s, index, 1))
		{
			diagRight = PieceAction(index, index - 7);
			legalMoves.push_back(diagRight);
		}		
		
		if(MoveForward(s, index, 1))
		{
			forward = PieceAction(index, index - 8);
			legalMoves.push_back(forward);
		}
		
		if(MoveDiagonalLeft(s, index, 1))
		{
			diagLeft = PieceAction(index, index - 9);
			legalMoves.push_back(diagLeft);
		}	
	}
	
	else {
		
		if(MoveDiagonalRight(s, index, 2))
		{
			diagRight = PieceAction(index, index + 7);
			legalMoves.push_back(diagRight);
		}		
		
		if(MoveForward(s, index, 2))
		{
			forward = PieceAction(index, index + 8);
			legalMoves.push_back(forward);
		}
		
		if(MoveDiagonalLeft(s, index, 2))
		{
			diagLeft = PieceAction(index, index + 9);
			legalMoves.push_back(diagLeft);
		}	
	}
	
	
	return legalMoves;
}

int GetNumActionsForPiece(BoardState s, int turn, int index)
{
	std::vector<PieceAction> ActionsForPiece = GetPieceActions(s, turn, index);
	return ActionsForPiece.size();
}



int GetNumActionsForState(BoardState s)
{
	if(s.playerTurn == 1)
	{
		return GetNumActions1(s);
	}
	
	else {
		return GetNumActions2(s);
	}
	
	return 0;
}

int GetNumActions1(BoardState s)
{
	return GetLegalActions1(s).size();
}

int GetNumActions2(BoardState s)
{
	return GetLegalActions2(s).size();
}

bool CheckCapture(BoardState s, PieceAction a)
{
	if(a.from == 1 && a.to == 2)
	{
		return true;
	}
	
	if(a.from == 2 && a.to == 1)
	{
		return true;
	}
	
	return false;
}

BoardState ApplyAction(BoardState s, PieceAction a)
{
	move_history.push_back(a);
	
	if(CheckCapture(s, a))
	{
		LastMoveCapture = true;
		
		if(s.playerTurn == 1)
		{
			num2 -= 1;
		}
		
		if(s.playerTurn == 2)
		{
			num1 -= 1;
		}
	}
	
	else {
		LastMoveCapture = false;
	}
	
	if(s.playerTurn == 1)
	{
		s.board[a.from] = 0;
		s.board[a.to] = 1;
	}
	
	if(s.playerTurn == 2)
	{
		s.board[a.from] = 0;
		s.board[a.to] = 2;
	}
	
	return s;
}

bool IsCapture(BoardState s, PieceAction a)
{
	if(s.playerTurn == 1 && a.from == 1 && a.to == 2)
	{
		return true;
	}
	
	if(s.playerTurn == 2 && a.from == 2 && a.to == 1)
	{
		return true;
	}
	
	return false;
}

uint64_t ZobristApplyAction(uint64_t hash, int turn, PieceAction a)
{
	BoardState s = GetStateFromHash(hash);
	if(IsCapture(s, a))
	{
		LastMoveCapture = true;
		
		if(turn == 1)
		{
			hash ^= zobrist_P2[a.to];
			num2 -= 1;
		}
		
		else if(turn == 2)
		{
			hash ^= zobrist_P1[a.to];
			num1 -= 1;
		}
	}
	
	else {
		LastMoveCapture = false;
	}
	
	if(turn == 1)
	{
		hash ^= zobrist_P1[a.from];
		hash ^= zobrist_P1[a.to];
	}
	
	else if(turn == 2)
	{
		hash ^= zobrist_P2[a.from];
		hash ^= zobrist_P2[a.to];
	}	
	
	return hash;
}


BoardState UndoAction(BoardState s, PieceAction a)
{
	move_history.pop_back();
	
	if(LastMoveCapture)
	{
		if(s.playerTurn == 1)
		{
			s.board[a.from] = 1;
			s.board[a.to] = 2;
			
			num2 += 1;
		}
		
		if(s.playerTurn == 2)
		{
			s.board[a.from] = 2;
			s.board[a.to] = 1;
			
			num1 += 1;
		}
	}
	
	else
	{
		if(s.playerTurn == 1)
		{
			s.board[a.from] = 1;
			s.board[a.to] = 0;
		}
		
		if(s.playerTurn == 2)
		{
			s.board[a.from] = 2;
			s.board[a.to] = 0;
		}
	}
	
	return s;
}

uint64_t ZobristUndoAction(uint64_t hash, int turn, PieceAction a)
{
	if(turn == 1)
	{
		hash ^= zobrist_P1[a.from];
		hash ^= zobrist_P1[a.to];
	}
	
	else if(turn == 2)
	{
		hash ^= zobrist_P2[a.from];
		hash ^= zobrist_P2[a.to];
	}	
	
	if(LastMoveCapture)
	{
		if(turn == 1)
		{
			hash ^= zobrist_P2[a.to];
			num2 += 1;
		}
		
		else if(turn == 2)
		{
			hash ^= zobrist_P1[a.to];
			num1 += 1;
		}
	}
	
	return hash;
}

int GetIndex(int x, int y)
{
	return y * 8 + x - 1;
}

int GetColumn(int index)
{
	return index % 8;
}

int GetRow(int index)
{
	return index / 8;
}

void PrintBoard(BoardState currentBoard)
{
	int line = 1;
	
	std::cout << "   A B C D E F G H" << std::endl;
	std::cout << line << "| ";
	
	for(int i = 0; i < 64; i++)
	{
		if(i == 8 * line)
		{
			std::cout << std::endl;
			line ++;
			std::cout << line << "| ";
		}
		if (currentBoard.board[i] == 1) {
			std::cout << "1 ";
		}
		else if (currentBoard.board[i] == 2) {
			std::cout << "2 ";
		}
		
		else {
			std::cout << "O ";
		}
	}
	
	std::cout << "\n";
	
	//	std::vector<PieceAction> moves = GetLegalActions2(currentBoard);
	
	//	for (std::vector<PieceAction>::size_type i = 0; i < moves.size(); ++i)
	//		std::cout << "Move from: " << moves[i].from	<< " to: " << moves[i].to << std::endl;
	
	std::cout << "\n";
}

int GetNumPiecesForState(BoardState s)
{	
	if(s.playerTurn == 1)
	{
		return GetNumPieces1(s);
	}
	
	else {
		return GetNumPieces2(s);
	}
	
	return 0;
}

int GetNumPieces1(BoardState s)
{
	int numPieces = 0;
	
	for(int i = 0; i < 64; i++)
	{
		if(s.board[i] == 1)
		{
			numPieces ++;
		}
	}
	
	return numPieces;
}

int GetNumPieces2(BoardState s)
{
	int numPieces = 0;
	
	for(int i = 0; i < 64; i++)
	{
		if(s.board[i] == 2)
		{
			numPieces ++;
		}
	}
	
	return numPieces;
}

void SwitchTurn(BoardState s)
{
	if(s.playerTurn == 1)
	{
		s.playerTurn = 2;
		Play(s);
	}
	
	else {
		s.playerTurn = 1;
		//		DoMove();
	}
	
}

int ConvertChar(char x)
{
	switch(x)
	{
		case 'A':
			return 1;
		case 'B':
			return 2;
		case 'C':
			return 3;
		case 'D':
			return 4;
		case 'E':
			return 5;
		case 'F':
			return 6;
		case 'G':
			return 7;
		case 'H':
			return 8;
	}
	
	return 0;
}
