/*
 *  Final Eval.h
 *  Breakthrough Improved Alpha Beta
 *
 *  Created by K Donegan on 3/11/13.
 *  Copyright 2013 University of Denver. All rights reserved.
 *
 */

#ifndef FINAL EVAL_H
#define FINAL EVAL_H

#include <iostream>
#include <vector>
#include <string>
#include "GameState.h"

int Evaluation(GameState s);

class Eval
{
public:
	bool CanWin(GameState s);
	bool CanCaptureLeft(GameState s, int index, Players turn);
	bool CanCaptureRight(GameState s, int index, Players turn);
	bool InDanger(GameState s, int index, Players turn);
	bool CanBeCounterCapturedLeft(GameState s, int index, Players turn);
	bool CanBeCounterCapturedRight(GameState s, int index, Players turn);
	bool CanBeCounterCaptured(GameState s, int index, Players turn);
	bool CanSupportLeft(GameState s, int index, Players turn);
	bool CanSupportRight(GameState s, int index, Players turn);
	bool CanSupportForward(GameState s, int index, Players turn);
	bool CanPreventCaptureLeft(GameState s, int index, Players turn);
	
};

#endif