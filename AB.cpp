/*
 *  AB.cpp
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/11/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */

#include "AB.h"

/*
 *  Misc.cpp
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/11/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */
/*
static class MoveSearchTree
{
	internal static int NodesSearchedLast = 0;
	internal static byte PlyDepth = 50; // Max-Ply Search
	internal static int TimeToMoveMs = 5000; // In milliseconds
	internal static Stopwatch Watch = new Stopwatch();
	internal static GamePieceColor WhosMove = GamePieceColor.White;
	
	private const bool EnableNullMoves = true;
	private const int Infinity = BoardEvaluation.WinValue;
	private const int TimeEnded = -9999999;
	private static int MovesNumber = 22;
	private static List<BoardState> ResultBoards;
	
	
	
	static MoveSearchTree()
	{
	}
	
	//region Possible boards retrieval
	/// <summary>
	/// Examines a board ang yields all possible boards
	/// </summary>
	private static std::vector<BoardState> GetPossibleBoards(int CurrentPlayer, BoardState ExamineBoard)
	{
		//We are going to store our result boards here          
		ResultBoards = new std::vector<BoardState>(MovesNumber);
		
		
		for (int x = 0; x < 64; x++)
		{
			//      for (int y = 0; y < 8; y++)
			//     {
			int Position = GetPosition(x, y);
			//Make sure there is a piece on the square
			if (ExamineBoard.board[x] != CurrentPlayer)
				continue;
			
			//Make sure the playerTurn is the same playerTurn as the one we are moving.
			//           if (ExamineBoard.BoardSquares[Position].CurrentPiece.PieceColor != ColorMoving)
			//              continue;
			
			//For each valid move for this piece
			//                   foreach (BoardPosition move in ExamineBoard.BoardSquares[Position].CurrentPiece.ValidMoves)
			else
			{
				for(int k = 0; k < GetNumActionsForPiece(ExamineBoard, CurrentPlayer, x); k++)
				{
					
					
					//                    int BPosition = GetPosition(move.BoardColumn, move.BoardRow);
					//if (ExamineBoard.BoardSquares[BPosition].CurrentPiece == null && CapturesOnly)
					//    continue;
					
					// make copies of the board and move so that we can move it without effecting the parent board
					BoardState board = ExamineBoard.FastCopy();
					
					// make move so we can examine it
					Board.MovePiece(board, x, y, move.BoardColumn, move.BoardRow);
					
					// generate Valid Moves for Board
					GamePieceValidMoves.GenerateValidMoves(board);
					
					// calculate the board score
					BoardEvaluation.GetValue(board, ColorMoving);
					
					ResultBoards.Add(board);
				}
			}
		}
		
		MovesNumber = ResultBoards.Count + 5;
		return ResultBoards;
		
	}
	
	//endregion
	
	//region Alpha-Beta algorithm v.5
	
	/// <summary>
	/// Root for ID Enhanced Alpha-Beta
	/// </summary>
	static BoardState IterativeDeepeningAlphaBeta(BoardState ExamineBoard, int MaxDepth, int playerTurn, bool ShowDepth)
	{
		//Set AI
		WhosMove = playerTurn;
		
		//Empty the Transposition Table
		BoardTT.Table = new Dictionary<ulong, BoardTTEntry>();
		BoardState BestBoard = null;
		BoardState LastBoard = null;
		
		int Depth = 1;
		
		Watch.Reset();
		Watch.Start();
		for (Depth = 1; Depth < MaxDepth; Depth++)
		{
			LastBoard = AlphaBetaTTRoot(ExamineBoard, Depth, playerTurn, EnableNullMoves);
			if (Watch.ElapsedMilliseconds >= TimeToMoveMs || LastBoard == null)
				break; // timeout
			BestBoard = LastBoard;
		}
		Watch.Stop();
		
		if (ShowDepth)
			Console.WriteLine("AI> Depth searched = {0}", Depth);
		return BestBoard;
	}
	
	/// <summary>
	/// Root for Fixed Depth Alpha-Beta
	/// </summary>
	static BoardState FixedDepthAlphaBeta(BoardState ExamineBoard, int MaxDepth, int playerTurn, bool ShowDepth)
	{
		//Set AI
		WhosMove = playerTurn;
		
		//Trash Transposition Table
		BoardTT.Table = new Dictionary<ulong, BoardTTEntry>();
		return AlphaBetaTTRoot(ExamineBoard, MaxDepth, playerTurn, EnableNullMoves);
	}
	
	/// <summary>
	/// Line of Sight Search
	/// </summary>
	static BoardState LoSSearch(BoardState ExamineBoard, int MaxDepth, int playerTurn)
	{
		var Successors = GetPossibleBoards(playerTurn, ExamineBoard);
		BoardState Result = null;
		
		// Fill the search space
		List<byte> SearchSpace = new List<byte>();
		if (playerTurn == 1)
		{
			for (byte i = 8; i < 40; i++) // 32
				if (ExamineBoard.board[i] != null && ExamineBoard.board[i] == playerTurn)
					SearchSpace.Add(i);
		}
		else
		{
			for (byte i = 24; i < 56; i++) // 32
				if (ExamineBoard.BoardSquares[i].CurrentPiece != null && ExamineBoard.BoardSquares[i].CurrentPiece.PieceColor == GamePieceColor.Black)
					SearchSpace.Add(i);
		}
		
		if (SearchSpace.Count > 0)
		{
			foreach (var position in SearchSpace)
			{
				// Reduce to Line of sight
				BoardState Reduced = Board.ReduceBoard(ExamineBoard, (int)GetColumn(position), (int)GetRow(position));
				BoardState Last = MoveSearchTree.AlphaBetaTTRoot(Reduced, MaxDepth, playerTurn, false);
				
				if (Last != null)
				{
					if (Last.Value >= BoardEvaluation.WinValue)
					{ // Winning position found! 
						Result = Last;
						break;
					}
					else if (Result != null && Result.Value < Last.Value)
					{ // Better one found
						Result = Last;
					}
					else if (Result == null)
					{
						Result = Last;
					}
				}
			}
		}
		
		return Result;
	}
	
	/// <summary>
	/// Root entry of TT Alpha-Beta algorithm (Transposition Table enhanced)
	/// </summary>
	internal static Board AlphaBetaTTRoot(Board ExamineBoard, byte Depth, int playerTurn, bool AllowNullMove)
	{
		// Perform variable initializations
		int BestValue = -Infinity;
		bool FirstCall = true;
		NodesSearchedLast = 0;
		Depth--;
		
		// Init hash (just first, next hashes will be rolled)
		ExamineBoard.GetHashValue();
		
		// Get all possible boards
		var Successors = GetPossibleBoards(playerTurn, ExamineBoard);
		var TotalBoards = Successors.Count; // count
		
		// Evaluate all and order the root
		Successors.Sort(Sort);
		
		// Result value initialization
		Board BestBoard = Successors[0];
		
		for (int i = 0; i < TotalBoards; i++)
		{
			var BoardToEvaluate = Successors[i];
			int Value;
			if (FirstCall)
			{ // First call, alpha = -infinity and beta = +infinity
				Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.Count), -Infinity, -BestValue, GetOppositeColor(playerTurn), AllowNullMove);
			}
			else
			{ // Better value found
				Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.Count), -BestValue - 1, -BestValue, GetOppositeColor(playerTurn), AllowNullMove);
				if (Value > BestValue)
				{
					Value = -AlphaBetaTT(BoardToEvaluate, ModifyDepth(Depth, Successors.Count), -Infinity, -BestValue, GetOppositeColor(playerTurn), AllowNullMove);
				}
			}
			
			if (Value > BestValue) // value is better
			{
				BestValue = Value;
				BestBoard = new Board(BoardToEvaluate);
				BestBoard.Value = Value;
				FirstCall = false;
			}
			
		}
		
		if (Watch.ElapsedMilliseconds >= TimeToMoveMs)
			return null; // no more time
		
		return BestBoard;
	}
	
	/// <summary>
	/// Alpha-Beta algorithm iteration
	/// </summary>
	private static int AlphaBetaTT(BoardState ExamineBoard, int Depth, int Alpha, int Beta, int playerTurn, bool AllowNullMove)
	{
		// Check stopwatch
		if (Watch.ElapsedMilliseconds >= TimeToMoveMs)
			return TimeEnded; // no more time
		
		var HashValue = ExamineBoard.HashValue;
		BoardTTEntry TTEntry;
		var Contains = BoardTT.Table.TryGetValue(HashValue, out TTEntry);
		
		
		if (Contains && TTEntry.Depth >= Depth)
		{
			var TTEntryType = TTEntry.Type;
			
			if (TTEntryType == BoardTTEntryType.ExactValue) // stored value is exact
				return TTEntry.Value;
			if (TTEntryType == BoardTTEntryType.Lowerbound && TTEntry.Value > Alpha)
				Alpha = TTEntry.Value; // update lowerbound alpha if needed
			else if (TTEntryType == BoardTTEntryType.Upperbound && TTEntry.Value < Beta)
				Beta = TTEntry.Value; // update upperbound beta if needed
			if (Alpha >= Beta)
				return TTEntry.Value; // if lowerbound surpasses upperbound
		}
		if (Depth == 0 || ExamineBoard.IsEnded())
		{
			//-------- BoardEvaluation.GetValue(ExamineBoard,playerTurn);
			var value = ExamineBoard.Value + Depth; // add depth (since it's inverse)
			if (value <= Alpha) // a lowerbound value
				BoardTT.StoreEntry(HashValue, new BoardTTEntry(value, BoardTTEntryType.Lowerbound, Depth));
			else if (value >= Beta) // an upperbound value
				BoardTT.StoreEntry(HashValue, new BoardTTEntry(value, BoardTTEntryType.Upperbound, Depth));
			else // a true minimax value
				BoardTT.StoreEntry(HashValue, new BoardTTEntry(value, BoardTTEntryType.ExactValue, Depth));
			return value;
		}
		
		NodesSearchedLast++;
		
		//Apply null move restrictions
		if (Depth >= 2 && Beta < Infinity && AllowNullMove && ExamineBoard.Pieces > 15)
		{
			// Try null move
			int r = 1;
			if (Depth >= 4) r = 2;
			else if (Depth >= 7) r = 3;
			
			Board.MakeNullMove(ExamineBoard);
			int value = -AlphaBetaTT(ExamineBoard, (byte)(Depth - r - 1), -Beta, -Beta + 1, playerTurn, false);
			Board.UnmakeNullMove(ExamineBoard);
			
			if (value >= Beta)
			{
				BoardTT.StoreEntry(HashValue, new BoardTTEntry(value, BoardTTEntryType.Upperbound, Depth));
				return value;
			}
		}
		
		
		var Successors = GetPossibleBoards(playerTurn, ExamineBoard);
		int totalBoards = Successors.Count;
		
		if (totalBoards == 0)
			return ExamineBoard.Value;
		
		// sort the boards in order to have better pruning
		Successors.Sort(Sort);
		Depth--;
		
		int Best = -BoardEvaluation.WinValue - 1;
		
		Board BoardToEvaluate;
		for (int i = 0; i < totalBoards; i++)
		{
			BoardToEvaluate = Successors[i];
			int value = -AlphaBetaTT(BoardToEvaluate, Depth, -Beta, -Alpha, GetOppositeColor(playerTurn), EnableNullMoves);
			
			if (value > Best)
				Best = value;
			if (Best > Alpha)
				Alpha = Best;
			if (Best >= Beta)
				break;
		}
		
		if (Best <= Alpha) // a lowerbound value
			BoardTT.StoreEntry(HashValue, new BoardTTEntry(Best, BoardTTEntryType.Lowerbound, Depth));
		else if (Best >= Beta) // an upperbound value
			BoardTT.StoreEntry(HashValue, new BoardTTEntry(Best, BoardTTEntryType.Upperbound, Depth));
		else // a true minimax value
			BoardTT.StoreEntry(HashValue, new BoardTTEntry(Best, BoardTTEntryType.ExactValue, Depth));
		return Best;
	}
	
	
	
	private static GamePieceColor GetOppositeColor(int playerTurn) { if (playerTurn == GamePieceColor.Black) return GamePieceColor.White; else return GamePieceColor.Black; }
	private static byte ModifyDepth(byte depth, int PossibleMoves) { if (PossibleMoves < 9) depth = (byte)(depth + 2); return depth; }
	private static int Sort(Board board1, Board board2) { return board2.Value - board1.Value; }
	private static byte GetRow(byte position) { return (byte)(7 - (int)(position / 8)); }
	private static byte GetColumn(byte position) { return (byte)(position % 8); }
	private static int GetPosition(int column, int row) { return (7 - row) * 8 + column; }
}


*/
