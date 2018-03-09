/*
 *  Eval.cpp
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/9/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */

#include "Eval.h"


#include <iostream>
#include <vector>


int Evaluation(BoardState s);
bool CanWin(BoardState s);
bool CanCaptureLeft(BoardState s, int index, int turn);
bool CanCaptureRight(BoardState s, int index, int turn);
bool InDanger(BoardState s, int index, int turn);
bool CanBeCounterCapturedLeft(BoardState s, int index, int turn);
bool CanBeCounterCapturedRight(BoardState s, int index, int turn);
bool CanBeCounterCaptured(BoardState s, int index, int turn);
bool CanSupportLeft(BoardState s, int index, int turn);
bool CanSupportRight(BoardState s, int index, int turn);
bool CanSupportForward(BoardState s, int index, int turn);
bool CanPreventCaptureLeft(BoardState s, int index, int turn);

bool Capture(BoardState s, int index, int turn);


int Evaluation(BoardState s)
{
	int gamescore = 0;
	std::vector<int> PieceScores;
	int furthestpiece1 = 63;
	int furthestpiece2 = 0;
	
	//If I can win on this turn
	if(CanWin(s) && s.playerTurn == 1)
	{
		gamescore += 1000000;
	}
	
	if(CanWin(s) && s.playerTurn == 2)
	{
		gamescore -= 1000000;
	}
	
//	if(s.playerTurn == 1)
//	{
		//Number of my pieces
		gamescore += GetNumPieces1(s);
		
		//Number of opponent's pieces
		gamescore -= GetNumPieces2(s);
		
		//Number of my moves available
		gamescore += (GetNumActions1(s) * 1.5);
		
		//Number of opponent's moves available
		gamescore -= (GetNumActions2(s) * 1.5);

//	}
	
//	if(s.playerTurn == 2)
//	{
		//Number of my pieces
//		gamescore += GetNumPieces2(s);
		
		//Number of opponent's pieces
//		gamescore -= GetNumPieces1(s);
		
		//Number of my moves available
//		gamescore += (GetNumActions2(s) * 1.5);
//	}
	
	
	
	
	//Distance from other side
	for(int i = 0; i < 64; i++)
	{
//		if(s.playerTurn == 1)
//		{
			if(s.board[i] == 1 && GetRow(i) > GetRow(furthestpiece1))
			{
				furthestpiece1 = i;
			}
//		}
		
//		if(s.playerTurn == 2)
//		{
			if(s.board[i] == 2 && GetRow(i) < GetRow(furthestpiece2))
			{
				furthestpiece2 = i;
			}
//		}
	}
	
//	if(s.playerTurn == 1)
//	{
		gamescore += 8 - GetRow(furthestpiece1) * 2;
//	}
//	else
//	{
		gamescore -= GetRow(furthestpiece2) * 2;
//	}
	
	//If I'm able to capture and whether that will lead to me being counter captured
	for(int i = 0; i < 64; i++)
	{
		if(CanCaptureLeft(s, i, s.playerTurn) && CanBeCounterCapturedLeft(s, i, s.playerTurn))
		{
			gamescore += 10;
		}
		
		if(CanCaptureRight(s, i, s.playerTurn) && CanBeCounterCapturedRight(s, i, s.playerTurn))
		{
			gamescore += 10;
		}
		
		if(CanCaptureLeft(s, i, s.playerTurn) && CanBeCounterCapturedLeft(s, i, s.playerTurn))
		{
			gamescore += 10;
		}
		
		if(CanCaptureRight(s, i, s.playerTurn) && CanBeCounterCapturedRight(s, i, s.playerTurn))
		{
			gamescore += 10;
		}
	}
	
	//If I'm able to capture and if I can't be counter captured
	for(int i = 0; i < 64; i++)
	{
		if(CanCaptureLeft(s, i, s.playerTurn) && !CanBeCounterCapturedLeft(s, i, s.playerTurn))
		{
			gamescore += 1000;
		}
		
		if(CanCaptureRight(s, i, s.playerTurn) && !CanBeCounterCapturedRight(s, i, s.playerTurn))
		{
			gamescore += 1000;
		}
		
		if(CanCaptureLeft(s, i, s.playerTurn) && !CanBeCounterCapturedLeft(s, i, s.playerTurn))
		{
			gamescore += 1000;
		}
		
		if(CanCaptureRight(s, i, s.playerTurn) && !CanBeCounterCapturedRight(s, i, s.playerTurn))
		{
			gamescore += 1000;
		}
	}
	
	//If I'm able to defend/support another piece
	for(int i = 0; i < 64; i++)
	{
		if(CanSupportLeft(s, i, s.playerTurn))
		{
			if(s.playerTurn == 1)
			{
				if(InDanger(s, i - 18, 1) || InDanger(s, i - 16, 1))
				{
					gamescore += 200;
				}
				
				else
				{
					gamescore += 50;
				}
			}
			
			if(s.playerTurn == 2)
			{
				if(InDanger(s, i + 18, 2) || InDanger(s, i + 16, 2))
				{
					gamescore -= 200;
				}
				
				else
				{
					gamescore -= 50;
				}
			}
		}
		
		if(CanSupportRight(s, i, s.playerTurn))
		{
			if(s.playerTurn == 1)
			{
				if(InDanger(s, i - 16, 1) || InDanger(s, i - 14, 1))
				{
					gamescore += 200;
				}
				
				else
				{
					gamescore += 50;
				}
			}
			
			if(s.playerTurn == 2)
			{
				if(InDanger(s, i + 16, 2) || InDanger(s, i + 14, 2))
				{
					gamescore -= 200;
				}
				
				else
				{
					gamescore -= 50;
				}
			}
		}
		
		if(CanSupportForward(s, i, s.playerTurn))
		{
			if(s.playerTurn == 1)
			{
				if(InDanger(s, i - 17, 1) || InDanger(s, i - 15, 1))
				{
					gamescore += 200;
				}
				
				else
				{
					gamescore += 50;
				}
			}
			
			if(s.playerTurn == 2)
			{
				if(InDanger(s, i + 17, 2) || InDanger(s, i + 15, 2))
				{
					gamescore -= 200;
				}
				
				else
				{
					gamescore -= 50;
				}
			}
		}
	}
	
	return gamescore;
}

bool CanWin(BoardState s)
{
	for(int i = 0; i < 64; i++)
	{
		if(s.playerTurn == 1)
		{
			if(s.board[i] == 1 && i < 16)
			{
				if(MoveDiagonalLeft(s, i, 1) || MoveForward(s, i, 1) || MoveDiagonalLeft(s, i, 1))
				{
					return true;
				}
			}
		}
		
		if(s.playerTurn == 2)
		{
			if(s.board[i] == 2 && i > 47)
			{
				if(MoveDiagonalLeft(s, i, 2) || MoveForward(s, i, 2) || MoveDiagonalLeft(s, i, 2))
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

bool CanCaptureLeft(BoardState s, int index, int turn)
{
	if(MoveDiagonalLeft(s, index, turn))
	{
		if(turn == 1 && s.board[index - 9] == 2)
		{
			return true;
		}
		
		if(turn == 2 && s.board[index + 9] == 1)
		{
			return true;
		}
	}
	
	return false;
}

bool CanCaptureRight(BoardState s, int index, int turn)
{
	if(MoveDiagonalRight(s, index, turn))
	{
		if(turn == 1 && s.board[index - 7] == 2)
		{
			return true;
		}
		
		if(turn == 2 && s.board[index + 7] == 1)
		{
			return true;
		}
	}
	
	return false;
}

bool InDanger(BoardState s, int index, int turn)
{
	//If a piece at a particular index can be captured from either diagonal left or right
	if(turn == 1 && s.board[index] == 1)
	{
		if(CanCaptureLeft(s, index - 9, 2) || CanCaptureRight(s, index - 7, 2))
		{
			return true;
		}
	}
	
	if(turn == 2 && s.board[index] == 2)
	{
		if(CanCaptureLeft(s, index + 9, 1) || CanCaptureRight(s, index + 7, 1))
		{
			return true;
		}
	}
	
	return false;
}

bool CanBeCounterCapturedLeft(BoardState s, int index, int turn)
{
	if(turn == 1)
	{
		//Can capture to the left and then be captured from the new left or right
		if(CanCaptureLeft(s, index, 1))
		{
			if(CanCaptureLeft(s, index - 18, 2) || CanCaptureRight(s, index - 16, 2))
			{
				return true;
			}			
		}
	}
	
	if(turn == 2)
	{
		//Can capture to the left and then be captured from the new left or right
		if(CanCaptureLeft(s, index, 2))
		{
			if(CanCaptureLeft(s, index + 18, 1) || CanCaptureRight(s, index + 16, 1))
			{
				return true;
			}			
		}
	}
	return false;
}

bool CanBeCounterCapturedRight(BoardState s, int index, int turn)
{
	if(turn == 1)
	{
		//Can capture to the left and then be captured from the new left or right
		if(CanCaptureRight(s, index, 1))
		{
			if(CanCaptureLeft(s, index - 16, 2) || CanCaptureRight(s, index - 14, 2))
			{
				return true;
			}			
		}
	}
	
	if(turn == 2)
	{
		//Can capture to the left and then be captured from the new left or right
		if(CanCaptureRight(s, index, 2))
		{
			if(CanCaptureLeft(s, index + 16, 1) || CanCaptureRight(s, index + 14, 1))
			{
				return true;
			}			
		}
	}
	return false;
}


bool CanBeCounterCaptured(BoardState s, int index, int turn)
{
	if(turn == 1)
	{
		//Can capture to the left and then be captured from the new left or right
		if(CanCaptureLeft(s, index, 1))
		{
			if(CanCaptureLeft(s, index - 18, 2) || CanCaptureRight(s, index - 16, 2))
			{
				return true;
			}			
		}
		
		//Can capture to the right and then be captured from the new left or right
		if(CanCaptureRight(s, index, 1))
		{
			if(CanCaptureLeft(s, index - 16, 2) || CanCaptureRight(s, index - 14, 2))
			{
				return true;
			}			
		}
	}
	
	if(turn == 2)
	{
		//Can capture to the left and then be captured from the new left or right
		if(CanCaptureLeft(s, index, 2))
		{
			if(CanCaptureLeft(s, index + 18, 1) || CanCaptureRight(s, index + 16, 1))
			{
				return true;
			}			
		}
		
		//Can capture to the right and then be captured from the new left or right
		if(CanCaptureRight(s, index, 2))
		{
			if(CanCaptureLeft(s, index + 16, 1) || CanCaptureRight(s, index + 14, 1))
			{
				return true;
			}			
		}
	}
	
	return false;
}

bool CanSupportLeft(BoardState s, int index, int turn)
{
	//If a piece at index can move to its diagonal left and is then diagonally left or right
	//behind another allied piece
	if(turn == 1)
	{
		if(MoveDiagonalLeft(s, index, 1))
		{
			if(s.board[index - 18] == 1 || s.board[index - 16] == 1)
			{
				return true;
			}
		}
	}
	
	if(turn == 2)
	{
		if(MoveDiagonalLeft(s, index, 2))
		{
			if(s.board[index + 18] == 2 || s.board[index + 16] == 2)
			{
				return true;
			}
		}
	}
	
	return false;
}

bool CanSupportRight(BoardState s, int index, int turn)
{
	//If a piece at index can move to its diagonal right and is then diagonally left or right
	//behind another allied piece
	if(turn == 1)
	{
		if(MoveDiagonalRight(s, index, 1))
		{
			if(s.board[index - 16] == 1 || s.board[index - 14] == 1)
			{
				return true;
			}
		}
	}
	
	if(turn == 2)
	{
		if(MoveDiagonalRight(s, index, 2))
		{
			if(s.board[index + 16] == 2 || s.board[index + 14] == 2)
			{
				return true;
			}
		}
	}
	
	return false;
}

bool CanSupportForward(BoardState s, int index, int turn)
{
	//If a piece at index can move forward and is then diagonally left or right
	//behind another allied piece
	if(turn == 1)
	{
		if(MoveForward(s, index, 1))
		{
			if(s.board[index - 17] == 1 || s.board[index - 15] == 1)
			{
				return true;
			}
		}
	}
	
	if(turn == 2)
	{
		if(MoveForward(s, index, 2))
		{
			if(s.board[index + 17] == 2 || s.board[index + 15] == 2)
			{
				return true;
			}
		}
	}
	
	return false;
}

bool CanPreventCaptureLeft(BoardState s, int index, int turn)
{
	if(turn == 1)
	{
		if(CanSupportLeft(s, index, 1))
		{
			if(InDanger(s, index - 18, 1) || InDanger(s, index - 16, 1))
			{
				return true;
			}
		}
	}
	
	return false;
}

