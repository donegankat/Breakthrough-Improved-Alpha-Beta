/*
 *  Alpha Beta.cpp
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/10/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */

#include "Alpha Beta.h"
#include "Game.h"
#include "ZobristValues.h"
#include "Timer.h"

#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <map>

Timer *myTimer = new Timer();
const double MaxMoveTime = 5;

static int alpha = Evaluation(GetStartState()); //Minimum
static int beta = 100000000; //Maximum
static PieceAction best_move, worst_move;
//static int depth = 0;
const int maxDepth = 2;
const int Infinity = INT_MAX;
const int WinValue = INT_MAX;
static int CurrentTurn = 1;

static BoardState BestBS;

static int NodesSearchedLast = 0;
static int PlyDepth = 50; // Max-Ply Search
static std::vector<BoardState> ResultBoards;

static std::vector<PieceAction> moves_taken1, moves_taken2;
static std::vector<PieceAction> best_sequence, worst_sequence;

static int GetNodesSearchedLast();
bool P1Win(BoardState s);
bool P2Win(BoardState s);
bool GameOver(BoardState s);

static int GetOppositeTurn(int turn);
int GetOppositePlayer(int player);
//static int GetRow(int position);
//static int GetColumn(int position);
//static int GetPosition(int c, int r);
static BoardState CopyBoard(BoardState s, int player);

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
bool updateHistory(std::vector<PieceAction> &history, PieceAction newmove);
void Run(BoardState &s, int turn, int depth, int a, int b);
void playMoves(BoardState &s);

void DoMove();
void test(std::vector<PieceAction> x, BoardState s);

void RunAlphaBeta(BoardState &s, int turn, int depth, int a, int b);
void StoreEntry(uint64_t Key, TTEntry Entry);

//BoardTT *TransTable = new BoardTT();
std::map<uint64_t,TTEntry> Table;

void StoreEntry(uint64_t Key, TTEntry Entry)
{
	if (Table.count(Key) == 0)
	{
		Table.insert(std::pair<uint64_t, TTEntry>(Key, Entry));
	}
	else {
		Table.erase(Key);
		Table.insert(std::pair<uint64_t, TTEntry>(Key, Entry));
	}
}

static int GetNodesSearchedLast()
{
	return NodesSearchedLast;
}

int GetNum1(BoardState s)
{
	int num1;
	
	for(int i = 0; i < 64; i++)
	{
		if(s.board[i] == 1)
		{
			num1 ++;
		}
	}
	
	return num1;
}

int GetNum2(BoardState s)
{
	int num2;
	
	for(int i = 0; i < 64; i++)
	{
		if(s.board[i] == 2)
		{
			num2 ++;
		}
	}
	
	return num2;
}

bool P1Win(BoardState s)
{
	
	if(GetNum2(s) == 0)//GetNumPieces2(s) == 0)
	{
		//std::cout << "Player 2 is out of pieces." << std::endl;
		return true;
	}	
	
	for(int i = 0; i < 8; i++)
	{
		if(s.board[i] == 1)
		{
			return true;
		}
	}
	
	return false;
}

bool P2Win(BoardState s)
{
	
	if(GetNum1(s) == 0)//GetNumPieces1(s) == 0)
	{
		//std::cout << "Player 1 is out of pieces." << std::endl;
		return true;
	}	
	
	for(int i = 56; i < 64; i++)
	{
		if(s.board[i] == 2)
		{
			return true;
		}
	}
	
	return false;
}

bool GameOver(BoardState s)
{
	if(P1Win(s) || P2Win(s))
	{
		return true;
	}
	
	return false;
}

int GetOppositePlayer(int player)
{
	if(player == 1)
	{
		return 2;
	}
	
	if(player == 2)
	{
		return 1;
	}
	
	return 0;
}

static BoardState CopyBoard(BoardState s, int player)
{
	BoardState Copied;// = new BoardState;
	Copied.playerTurn = player;
	
	for(int i = 0; i < 64; i++)
	{
		Copied.board[i] = s.board[i];
	}
	
	return Copied;
}

static int GetOppositeTurn(int turn)
{
	if (turn == 1)
		return 1;
	else
		return 2;
}

static uint64_t ZobristSwitchTurn(uint64_t hash, int turn)
{
	hash ^= zobrist_Turn[0];
	hash ^= zobrist_Turn[1];
	
	return hash;
}

static int ModifyDepth(int depth, int PossibleMoves)
{
	if(PossibleMoves < 9)
		depth = (depth + 2);
	return depth;
}

//static int Sort(BoardState board1, BoardState board2)
//{
//	return board2.Value - board1.Value;
//}

/*
static int GetRow(int position)
{
	return (7 - (position / 8));
}

static int GetColumn(int position)
{
	return (position % 8);
}

static int GetPosition(int column, int row)
{
	return (7 - row) * 8 + column;
}
*/

uint64_t MakeNullMove(BoardState board)
{
	//int val = Evaluation(board);
	//val = -val;
	//hash ^= zobrist_Null[0];
	
	uint64_t hash = GetHashFromState(board);
		
	hash ^= zobrist_Null[0];

	return hash;
	
}

uint64_t UnmakeNullMove(BoardState board)
{
	uint64_t hash = GetHashFromState(board);
	
	hash ^= zobrist_Null[0];
	
	return hash;
	
	//int val = Evaluation(board);
	//hash ^= zobrist_Null[0];
	//val = -val;
}


static std::vector<BoardState> PossibleBoards(BoardState s, int player)
{
	std::vector<int> MovablePieces; // = new std::vector<int>;
		
	for(int i = 0; i < 64; i++)
	{
		if(s.board[i] != player)
		{
			continue;
		}
		else //if(s.board[i] == player)
		{
			MovablePieces.push_back(i);
		}
	}
	for(int i = 0; i < MovablePieces.size(); i++)
	{
		BoardState CurrentBoard;
		std::vector<PieceAction> MovesForPiece;
		MovesForPiece = GetPieceActions(s, player, MovablePieces[i]);
		for(int j = 0; j < MovesForPiece.size(); j++)
		{
			CurrentBoard = ApplyAction(s, MovesForPiece[j]);
//			MovesForPiece.erase(MovesForPiece[j]);
//			PrintBoard(CurrentBoard);
			
			ResultBoards.push_back(CurrentBoard);
			
			//CurrentBoard = UndoAction(CurrentBoard, MovesForPiece[j]);
		}
		
	}
	
//	MovablePieces.clear();
	return ResultBoards;
		
}


std::vector<PieceAction> PossibleMoves(BoardState s, int player)
{
	std::vector<PieceAction> AllMoves;
	
//	for(int a = 0; a < s.size(); a++)
//	{
		std::vector<int> MovablePieces;
		
		for(int i = 0; i < 64; i++)
		{
			if(s.board[i] != player)
			{
				continue;
			}
			else
			{
				MovablePieces.push_back(i);
			}
			
		}
		
		for(int i = 0; i < MovablePieces.size(); i++)
		{
			std::vector<PieceAction> temp;
			temp = GetPieceActions(s, player, MovablePieces[i]);
			
			for(int j = 0; j < temp.size(); j++)
			{
				AllMoves.push_back(temp[j]);
			}
			
		}
//	}
	
	return AllMoves;
	
}


/// <summary>
/// Root for Fixed Depth Alpha-Beta
/// </summary>
static BoardState FixedDepthAlphaBeta(BoardState ExamineBoard, int MaxDepth, int playerTurn)
{
	//Set AI
	CurrentTurn = playerTurn;
	
	//Trash Transposition Table
	Table.clear();
	return AlphaBetaTTRoot(ExamineBoard, MaxDepth, playerTurn, true);
}

PieceAction FixedDepthAlphaBetaMove(BoardState ExamineBoard, int MaxDepth, int playerTurn)
{
	//Set AI
	CurrentTurn = playerTurn;
	
	//Trash Transposition Table
	Table.clear();
	return AlphaBetaTTRootMove(ExamineBoard, MaxDepth, playerTurn, true);
}

/// <summary>
/// Root entry of TT Alpha-Beta algorithm (Transposition Table enhanced)
/// </summary>

bool SortSuccessors(BoardState i, BoardState j)
{
	return (Evaluation(i) > Evaluation(j));
}

static BoardState AlphaBetaTTRoot(BoardState ExamineBoard, int Depth, int turn, bool AllowNullMove)
{
	// Perform variable initializations
	int BestValue = -Infinity;
	bool FirstCall = true;
	NodesSearchedLast = 0;
	Depth--;
	
	// Init hash (just first, next hashes will be rolled)
	ExamineBoard.playerTurn = turn;
	
	// Get all possible boards
	std::vector<BoardState> Successors = PossibleBoards(ExamineBoard, turn);
	int TotalBoards = Successors.size(); // count

	// Get all possible moves
	std::vector<PieceAction> SuccessorMoves = PossibleMoves(ExamineBoard, turn);
	int TotalMoves = SuccessorMoves.size(); // count
	
	//std::cout << "Boards: " << TotalBoards << " Moves: " << TotalMoves << std::endl;
	
	// Evaluate all and order the root
	std::sort(Successors.begin(), Successors.end(), SortSuccessors);
	
	// Result value initialization
	BoardState BestBoard = Successors[0];
	PieceAction BestMove = SuccessorMoves[0];
	
	for (int i = 0; i < TotalBoards; i++)
	{
		BoardState BoardToEvaluate = Successors[i];
		int Value;
		
		if (FirstCall)
		{ // First call, alpha = -infinity and beta = +infinity
			Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.size()), -Infinity, -BestValue, GetOppositeTurn(turn), AllowNullMove);
		}
		else
		{ // Better value found
			Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.size()), -BestValue - 1, -BestValue, GetOppositeTurn(turn), AllowNullMove);
			if (Value > BestValue)
			{
				Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.size()), -Infinity, -BestValue, GetOppositeTurn(turn), AllowNullMove);
			}
		}
		
		if (Value > BestValue) // value is better
		{
			BestValue = Value;
			BestBoard = BoardToEvaluate;
			BestMove = SuccessorMoves[i];
			//BestBoard.Value = Value;
			FirstCall = false;
		}
		
	}
	

//	if (Watch.ElapsedMilliseconds >= TimeToMoveMs)
//		return null; // no more time
	
	return BestBoard;
}
								

PieceAction AlphaBetaTTRootMove(BoardState ExamineBoard, int Depth, int turn, bool AllowNullMove)
{
	// Perform variable initializations
	int BestValue = -Infinity;
	bool FirstCall = true;
	NodesSearchedLast = 0;
	Depth--;
	
	// Init hash (just first, next hashes will be rolled)
	ExamineBoard.playerTurn = turn;
	
	// Get all possible boards
	std::vector<BoardState> Successors = PossibleBoards(ExamineBoard, turn);
	int TotalBoards = Successors.size(); // count
	
	// Get all possible moves
	std::vector<PieceAction> SuccessorMoves = PossibleMoves(ExamineBoard, turn);
	int TotalMoves = SuccessorMoves.size(); // count
	
	//std::cout << "Boards: " << TotalBoards << " Moves: " << TotalMoves << std::endl;
	
	// Evaluate all and order the root
	std::sort(Successors.begin(), Successors.end(), SortSuccessors);
	
	// Result value initialization
	BoardState BestBoard = Successors[0];
	PieceAction BestMove = SuccessorMoves[0];
	
	for (int i = 0; i < TotalBoards; i++)
	{
		BoardState BoardToEvaluate = Successors[i];
		int Value;
		
		if (FirstCall)
		{ // First call, alpha = -infinity and beta = +infinity
			Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.size()), -Infinity, -BestValue, GetOppositeTurn(turn), AllowNullMove);
		}
		else
		{ // Better value found
			Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.size()), -BestValue - 1, -BestValue, GetOppositeTurn(turn), AllowNullMove);
			if (Value > BestValue)
			{
				Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.size()), -Infinity, -BestValue, GetOppositeTurn(turn), AllowNullMove);
			}
		}
		
		if (Value > BestValue) // value is better
		{
			BestValue = Value;
			BestBoard = BoardToEvaluate;
			BestMove = SuccessorMoves[i];
			//BestBoard.Value = Value;
			FirstCall = false;
		}
		
	}
	
	//	if (Watch.ElapsedMilliseconds >= TimeToMoveMs)
	//		return null; // no more time
	
	return BestMove;
}

								
		
								
								
/// <summary>
/// Alpha-Beta algorithm iteration
/// </summary>

static int AlphaBetaTT(BoardState ExamineBoard, int Depth, int Alpha, int Beta, int turn, bool AllowNullMove)
{
	// Check time
	double duration = myTimer->TimeElapsed(*myTimer);
	
	if (duration >= MaxMoveTime)
	{
		//std::cout << "AlphaBetaTT() has timed out" << std::endl;
		return 0; // timeout
	}
	
	// Search the Transposition Table for this state's hash
	uint64_t HashValue = GetHashFromState(ExamineBoard);
	TTEntry theEntry;
	bool EntryInTable = false;
	
	std::map<uint64_t,TTEntry>::iterator it;
	for(std::map<uint64_t,TTEntry>::iterator it=Table.begin(); it!=Table.end(); ++it)
	{
		if(it->first == HashValue)
		{
			EntryInTable = true;
			//std::cout << "Entry found - retrieving data" << std::endl;
			theEntry = it->second;
		}
	}
	
	if (EntryInTable && theEntry.Depth >= Depth)
	{
		TTEntryType theType= theEntry.Type;
		
		if (theType == ExactValue) // stored value is exact
			return theEntry.Value;
		if (theType == LowerBound && theEntry.Value > Alpha)
			Alpha = theEntry.Value; // update lowerbound alpha if needed
		else if (theType == UpperBound && theEntry.Value < Beta)
			Beta = theEntry.Value; // update upperbound beta if needed
		if (Alpha >= Beta)
			return theEntry.Value; // if lowerbound surpasses upperbound
	}
	if (Depth == 0 || GameOver(ExamineBoard))
	{
		int value = Evaluation(ExamineBoard) + Depth; // add depth (since it's inverse)
		TTEntry newEntry;
		newEntry.Value = value;
		newEntry.Depth = Depth;
		
		if (value <= Alpha) // a LowerBound value
		{
			newEntry.Type = LowerBound;
		}
		else if (value >= Beta) // an UpperBound value
			newEntry.Type = UpperBound;
		else // a true minimax value
			newEntry.Type = ExactValue;
		
		StoreEntry(HashValue, newEntry);
		return value;
		
	}
	
	NodesSearchedLast++;
	
	//Apply null move restrictions
	if (Depth >= 2 && Beta < Infinity && AllowNullMove && (GetNumPieces1(ExamineBoard) + GetNumPieces2(ExamineBoard)) > 15)
	{
		// Try null move
		int r = 1;
		if (Depth >= 4) r = 2;
		else if (Depth >= 7) r = 3;
		
		MakeNullMove(ExamineBoard);
		int value = -AlphaBetaTT(ExamineBoard, (Depth - r - 1), -Beta, -Beta + 1, turn, false);
		UnmakeNullMove(ExamineBoard);
		
		if (value >= Beta)
		{
			TTEntry newEntry;
			newEntry.Value = value;
			newEntry.Depth = Depth;
			newEntry.Type = UpperBound;
			StoreEntry(HashValue, newEntry);
			
			return value;
		}
	}
	
	
	std::vector<BoardState> Successors = PossibleBoards(ExamineBoard, turn);
	int totalBoards = Successors.size();
	
	if (totalBoards == 0)
		return Evaluation(ExamineBoard);
	
	// sort the boards in order to have better pruning
	std::sort(Successors.begin(), Successors.end(), SortSuccessors);
	Depth--;
	
	int Best = -WinValue - 1;
	
	BoardState BoardToEvaluate;
	for (int i = 0; i < totalBoards; i++)
	{
		BoardToEvaluate = Successors[i];
		int value = -AlphaBetaTT(BoardToEvaluate, Depth, -Beta, -Alpha, GetOppositeTurn(turn), true);
		
		if (value > Best)
			Best = value;
		if (Best > Alpha)
			Alpha = Best;
		if (Best >= Beta)
			break;
	}
	
	TTEntry BestTTEntry;
	BestTTEntry.Value = Best;
	BestTTEntry.Depth = Depth;
	
	if (Best <= Alpha) // a LowerBound value
		BestTTEntry.Type = LowerBound;
	else if (Best >= Beta) // an UpperBound value
		BestTTEntry.Type = UpperBound;
	else // a true minimax value
		BestTTEntry.Type = ExactValue;
	
	StoreEntry(HashValue, BestTTEntry);
	
	return Best;
}

int NodesSearched;
std::vector<PieceAction> MoveHistory; // = new Stack<MoveContent>();

BoardState theBoard = GetStartState();
BoardState previousBoard;
int currentPlayer = 1;

void DoMove(BoardState s)
{
	//std::cout << "Player: " << s.playerTurn << std::endl;
	
	//only if not winning board
	if (s.board != 0 && !GameOver(s))
	{
		//std::cout << "Finding best board." << std::endl;
		//		Watch.Start();
		std::vector<BoardState> Tree;// = new List<Board>();
		
		NodesSearched = 0;
		BoardState BestBoard; // = new Board();
		//BestBoard = IterativeDeepeningAlphaBeta(theBoard, MoveSearchTree.PlyDepth, WhosMove, DoRender);
		BestBoard = FixedDepthAlphaBeta(s, 2, s.playerTurn);
		//BestBoard = MoveSearchTree.AlphaBetaRoot(ChessBoard, 7, WhosMove);
		
		for(int i = 0; i < MoveHistory.size(); i++)
		{
			//std::cout << MoveHistory[i].from << " " << MoveHistory[i].to << std::endl;
		}
		
		if (BestBoard.board != 0 && !MoveHistory.empty())
		{
			//std::cout << "Applying actions." << std::endl;
			PieceAction LastMove = MoveHistory.back();
			BestBoard = ApplyAction(s, LastMove);
			
			//PrintBoard(BestBoard);
			
			int BestValue = Evaluation(BestBoard);
		}
		else
		{
		}
		
		NodesSearched = GetNodesSearchedLast();
	}
}

std::vector<PieceAction> mv, bestaction, worstaction;
int maxd = 3;
static int result;
static std::vector<PieceAction> move_history;
static PieceAction move_to_take;


//std::vector<PieceAction> updateHistory(std::vector<PieceAction> history, PieceAction newmove)
bool updateHistory(std::vector<PieceAction> &history, PieceAction newmove)
{
	for(int i = 0; i < history.size(); i++)
	{
		if(newmove.depth == history[i].depth && newmove.player == history[i].player)
		{
			history[i].from = newmove.from;
			history[i].to = newmove.to;
			
			return true;
		}
	}
	
	return false;
}

void Run(BoardState &s, int turn, int depth, int a, int b)
{
//	std::cout << "Player: " << turn << std::endl;
	s.playerTurn = turn;
	std::vector<BoardState> Boards = PossibleBoards(s, turn);
	
	if(depth == 0)
	{
		alpha = -Infinity;
		beta = Infinity;
	}
	
	if(depth == maxd)
	{
		return;// Evaluation(s);
	}
	
	//std::cout << "Player: " << s.playerTurn << std::endl;
	//std::cout << "Player: " << turn << std::endl;
	if(turn == 1)
	{
		s.playerTurn = 1;
		std::vector<PieceAction> Moves = GetLegalActions1(s);

		for(int i = 0; i < Moves.size(); i++)
		{
			PieceAction tempMove = PieceAction(Moves[i].from, Moves[i].to, depth, 1);
			BoardState temp = ApplyAction(s, Moves[i]);
			
			if(alpha >= beta)
			{
				bool updated = false;
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
						
						//std::cout << "Updated move from: " << tempMove.from << " to: " << tempMove.to << std::endl;
						
						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
				
					move_history.push_back(tempMove);
				}
				
				result = Evaluation(temp);
				//std::cout << "Player 1 Result: " << result << std::endl;
				//alpha = Evaluation(temp);
				//return Evaluation(temp);
			}
			
			if(alpha < Evaluation(temp))
			{
				bool updated = false;
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
						
						//std::cout << "Updated move from: " << tempMove.from << " to: " << tempMove.to << std::endl;
						
						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
				 
					move_history.push_back(tempMove);
				}				
				alpha = Evaluation(temp);
				//std::cout << "NEW ALPHA VALUE: " << alpha << std::endl;
			}
			
			else
			{
				break;
			}
			
			temp.playerTurn = 2;
			Run(temp, 2, depth + 1, alpha, beta);
			
			UndoAction(temp, Moves[i]);
		}
		
		Moves.clear();
		return;// alpha;
	}
	
	if(turn == 2)
	{
		s.playerTurn = 2;
		std::vector<PieceAction> Moves = GetLegalActions2(s);
		
		for(int i = 0; i < Moves.size(); i++)
		{
			PieceAction tempMove = PieceAction(Moves[i].from, Moves[i].to, depth, 2);
			BoardState temp = ApplyAction(s, Moves[i]);
			
			if(alpha >= beta)
			{
				bool updated = false;
				
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
						
						//std::cout << "Updated move from: " << tempMove.from << " to: " << tempMove.to << std::endl;

						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
				 
					move_history.push_back(tempMove);
				}
				
				result = Evaluation(temp);
				//std::cout << "Player 2 Result: " << result << std::endl;
			}
			
			if(beta > Evaluation(temp))
			{
				bool updated = false;
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
						
						//std::cout << "Updated move from: " << tempMove.from << " to: " << tempMove.to << std::endl;
						
						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
					move_history.push_back(tempMove);
				}
				
				beta = Evaluation(temp);
				//std::cout << "NEW BETA VALUE: " << beta << std::endl;
			}
			
			else
			{
				break;
			}
			
			temp.playerTurn = 1;
			Run(temp, 1, depth + 1, alpha, beta);
			
			UndoAction(temp, Moves[i]);
		}
		
		Moves.clear();
				
		return;// beta;
	}
	
//	if (s.board != 0 && !GameOver(s))
//	{
//		std::sort(Boards.begin(), Boards.end(), SortSuccessors);

//		if(s.playerTurn == 1)
//		{
//			P1LastScore = Evaluation(s);
//		}
//		if(s.playerTurn == 2)
//		{
//			P2LastScore = Evaluation(s);
//		}
		

		/*
			for(int j = 0; j < Boards.size(); j++)
			{
				if(s.playerTurn == 1)
				{
					if(P1LastScore == Evaluation(Boards[j]))
					{
						continue;
					}
					
					else
					{
						
						for(int i = 0; i < 64; i++)
						{
							s.board[i] = Boards[j].board[i];
						}
						P1LastScore = Evaluation(s);

					}
				}
				
				if(s.playerTurn == 2)
				{
					if(P2LastScore == Evaluation(Boards[j]))
					{
						continue;
					}
					
					else
					{
						
						for(int i = 0; i < 64; i++)
						{
							s.board[i] = Boards[j].board[i];
						}
						P2LastScore = Evaluation(s);
					}
				}
			}
		*/
		
//		int bindex = 0;
//		for(int j = 0; j < Boards.size(); j++)
//		{
//			if(Evaluation(s) == Evaluation(Boards[0]))
//			{
//				std::cout << "DUPLICATE" << std::endl;
//				return;
//				continue;
//			}
//			else {
//				bindex = j;
//				break;
//			}
//		}
		
		
//		for(int i = 0; i < 64; i++)
//		{
//			s.board[i] = Boards[bindex].board[i];
//		}

		/*
		for(int i = 0; i < Boards.size(); i++)
		{
			std::cout << "Start **************************************" << std::endl;
			PrintBoard(Boards[i]);
			std::cout << "************************************** End" << std::endl;
		}
		 */
//		std::cout << "Score of current state: " << Evaluation(s) << std::endl;
//		std::cout << "Depth: " << d << std::endl;
//		PrintBoard(s);
		
//		s.playerTurn = GetOppositePlayer(s.playerTurn);
//		Run(s, d + 1);
//	}

	if(GameOver(s))
	{
		//std::cout << "Game Over. ";
		if(P1Win(s))
		{
		//	std::cout << "Player 1 Wins!" << std::endl;
		}
		if(P2Win(s))
		{
		//	std::cout << "Player 2 Wins!" << std::endl;
		}
		
		return;
	}
		
	for(int i = 0; i < move_history.size(); i++)
	{
		std::cout << move_history[i].from << " " << move_history[i].to << std::endl;
	}
	
	playMoves(s);
	return;// 0;
}

void playMoves(BoardState &s)
{
	for(int i = 0; i < move_history.size(); i++)
	{
		s = ApplyAction(s, move_history[i]);
		//PrintBoard(s);
		
		if(s.playerTurn == 1)
		{
			s.playerTurn = 2;
		}
		else {
			s.playerTurn = 1;
		}
	}
}


std::vector<PieceAction> moves_to_take;
void RunAlphaBeta(BoardState &s, int turn, int depth, int a, int b)
{
	s.playerTurn = turn;
	//std::cout << "Player: " << s.playerTurn << std::endl;
	
	std::vector<BoardState> Boards = PossibleBoards(s, turn);
	
	if(depth == 0)
	{
		alpha = -Infinity;
		beta = Infinity;
	}
	
	if(depth == maxd)
	{
		//std::cout << "Reached max depth" << std::endl;
		return;// Evaluation(s);
	}
	
	if(GameOver(s))
	{
		//std::cout << "Game Over. ";
		if(P1Win(s))
		{
			//std::cout << "Player 1 Wins!" << std::endl;
		}
		if(P2Win(s))
		{
			//std::cout << "Player 2 Wins!" << std::endl;
		}
		
		return;
	}
	
	if(turn == 1)
	{
		s.playerTurn = 1;
		std::vector<PieceAction> Moves = GetLegalActions1(s);
		
		for(int i = 0; i < Moves.size(); i++)
		{
			PieceAction tempMove = PieceAction(Moves[i].from, Moves[i].to, depth, 1);
			BoardState temp = ApplyAction(s, Moves[i]);
			
			if(alpha >= beta)
			{
				bool updated = false;
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						//std::cout << "Updating move: " << move_history[j].from << ", " << move_history[j].to << " to: " << tempMove.from << ", " << tempMove.to << std::endl;
						
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
						
						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
					move_history.push_back(tempMove);
				}
				
				result = Evaluation(temp);
				//std::cout << "Player 1 Result: " << result << std::endl;
				
			}
			
			if(alpha < Evaluation(temp))
			{
				bool updated = false;
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						//std::cout << "Updating move: " << move_history[j].from << ", " << move_history[j].to << " to: " << tempMove.from << ", " << tempMove.to << std::endl;
						
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
												
						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
					move_history.push_back(tempMove);
				}				
				alpha = Evaluation(temp);
				//std::cout << "NEW ALPHA VALUE: " << alpha << std::endl;
			}
			
			else
			{
				break;
			}
			
			//PrintBoard(temp);
			
			temp.playerTurn = 2;
			RunAlphaBeta(temp, 2, depth + 1, alpha, beta);
			
			UndoAction(temp, Moves[i]);
		}
		
		Moves.clear();
		
//		std::cout << "Move history: " << std::endl;
//		for(int i = 0; i < move_history.size(); i++)
//		{
//			std::cout << "Index " << i << ": " << move_history[i].from << ", " << move_history[i].to << std::endl;
//		}
		
		return;
	}
	
	if(turn == 2)
	{
		s.playerTurn = 2;
		std::vector<PieceAction> Moves = GetLegalActions2(s);
		
		for(int i = 0; i < Moves.size(); i++)
		{
			PieceAction tempMove = PieceAction(Moves[i].from, Moves[i].to, depth, 2);
			BoardState temp = ApplyAction(s, Moves[i]);
			
			if(alpha >= beta)
			{
				bool updated = false;
				
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						//std::cout << "Updating move: " << move_history[j].from << ", " << move_history[j].to << " to: " << tempMove.from << ", " << tempMove.to << std::endl;
						
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
												
						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
					move_history.push_back(tempMove);
				}
				
				result = Evaluation(temp);
				//std::cout << "Player 2 Result: " << result << std::endl;
			}
			
			if(beta > Evaluation(temp))
			{
				bool updated = false;
				
				for(int j = 0; j < move_history.size(); j++)
				{
					if(depth == move_history[j].depth && turn == move_history[j].player)
					{
						//std::cout << "Updating move: " << move_history[j].from << ", " << move_history[j].to << " to: " << tempMove.from << ", " << tempMove.to << std::endl;
						
						move_history[j].from = tempMove.from;
						move_history[j].to = tempMove.to;
												
						updated = true;
						
						break;
					}
					
				}
				
				if(!updated)
				{
					move_history.push_back(tempMove);
				}
				
				beta = Evaluation(temp);
				//std::cout << "NEW BETA VALUE: " << beta << std::endl;
			}
			
			else
			{
				break;
			}

			//PrintBoard(temp);
			
			temp.playerTurn = 1;
			RunAlphaBeta(temp, 1, depth + 1, alpha, beta);
			
			UndoAction(temp, Moves[i]);
		}
		
		Moves.clear();
		
//		std::cout << "Move history: " << std::endl;
//		for(int i = 0; i < move_history.size(); i++)
//		{
//			std::cout << "Index " << i << ": " << move_history[i].from << ", " << move_history[i].to << std::endl;
//		}
		
		return;
	}
	
	return;
}

BoardState MyAB(BoardState s)
{
	
}

