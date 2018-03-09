/*
 *  Eval.h
 *  Breakthrough Alpha Beta
 *
 *  Created by K Donegan on 2/9/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */

#ifndef EVAL_H
#define EVAL_H

#include <iostream>
#include <vector>
#include <string>
#include "Game.h"

int Evaluation(BoardState s);
class Eval
{
public:
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
	
};

#endif