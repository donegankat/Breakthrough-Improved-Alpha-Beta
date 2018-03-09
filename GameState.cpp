#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <ostream>
using std::ostream;
#include <sstream>
using std::ostringstream;
#include <algorithm>

#include "GameState.h"
#include "Move.h"
#include "String.h"
#include "Timer.h"

Timer *myTimer = new Timer();
const double MaxMoveTime = 5;

const int Infinity = INT_MAX;
const int WinValue = INT_MAX;

static int NodesSearchedLast = 0;


ostream& operator<<(ostream& os, GameState& s)
{
    os << "--------------------------------------------------------------\n";

    // Add column labels
    os << "   ";
    for (char i = 'a'; i - 'a' < (char) s.board_size; ++i)
        os << i;
    os << "\n";

    // Don't start i at 0 to skip first row since it is out of bounds
    for (size_t i = s.board_size + 2; i < s.board.size() - (s.board_size + 2); ++i)
    {
        // Add row labels
        if (i % (s.board_size + 2) == 0)
        {
            os << " " << i / (s.board_size + 2) << " ";
            // Also they correspond to out of play positions
            continue;
        }

        // Skip if edge of border
        if (i % (s.board_size + 2) == s.board_size + 1)
        {
            os << "\n";
            continue;
        }

        switch(s.board[i])
        {
            case board_open:
                os << ".";
                break;
            case board_closed:
                os << "X";
                break;
            case board_player1:
                os << "1";
                break;
            case board_player2:
                os << "2";
                break;
        }
    }

    int p1_count = 0;
    int p2_count = 0;
    for (vector<Piece>::const_iterator i = s.pieces.begin(), e = s.pieces.end(); i != e; ++i)
        if (i->player == player1)
            ++p1_count;
        else
            ++p2_count;

    os << "There are " << s.pieces.size() << " pieces still in play. "
       << "Of them, " << p1_count << " are Player 1's "
       << "and " << p2_count << " Player 2's.\n";

    vector<Move> p1moves = s.get_moves(player1);
    os << "Player 1 has " << p1moves.size() << " moves: ";
    for (vector<Move>::const_iterator m = p1moves.begin(), e = p1moves.end(); m != e; ++m)
        os << s.pretty_print_move(*m) << ", ";
    os << "\n";

    vector<Move> p2moves = s.get_moves(player2);
    os << "Player 2 has " << p2moves.size() << " moves: ";
    for (vector<Move>::const_iterator m = p2moves.begin(), e = p2moves.end(); m != e; ++m)
        os << s.pretty_print_move(*m) << ", ";
    os << "\n";

    os << "--------------------------------------------------------------"
       << endl;

    return os;
}

GameState::GameState(size_t board_size_) : board_size(board_size_)
{
    reset();
}

void GameState::set_turn(Players new_player)
{
	turn = new_player;
}

void GameState::switch_turn()
{
	turn = (turn == player1) ? player2 : player1;
}

Move GameState::get_last_move() const
{
	return move_history.back();
}

int GameState::get_num_pieces_total() const
{
	return pieces.size();
}

int GameState::get_num_pieces1() const
{
	int p1_count = 0;
    for (vector<Piece>::const_iterator i = pieces.begin(), e = pieces.end(); i != e; ++i)
        if (i->player == player1)
            ++p1_count;
	return p1_count;
}

int GameState::get_num_pieces2() const
{
	int p2_count = 0;
    for (vector<Piece>::const_iterator i = pieces.begin(), e = pieces.end(); i != e; ++i)
        if (i->player == player2)
            ++p2_count;
	return p2_count;
}

vector<Move> GameState::get_moves() const
{
    return get_moves(turn);
}

vector<Move> GameState::get_moves(const Players player) const
{
    vector<Move> moves;

    for(vector<Piece>::const_iterator p = pieces.begin(), e = pieces.end(); p != e; ++p)
    {
        if (p->player == player && player == player1)
        {
            // Player 1
            size_t down_left = p->location + board_size + 1;
            if (board[down_left] == board_open ||
                board[down_left] == board_player2)
                moves.push_back(Move(p->location, down_left));

            size_t down = p->location + board_size + 2;
            if (board[down] == board_open)
                moves.push_back(Move(p->location, down));

            size_t down_right = p->location + board_size + 3;
            if (board[down_right] == board_open ||
                board[down_right] == board_player2)
                moves.push_back(Move(p->location, down_right));
        }
        else if (p->player == player && player == player2)
        {
            // Player 2
            size_t up_left = p->location - board_size - 3;
            if (board[up_left] == board_open ||
                board[up_left] == board_player1)
                moves.push_back(Move(p->location, up_left));

            size_t up = p->location - board_size - 2;
            if (board[up] == board_open)
                moves.push_back(Move(p->location, up));

            size_t up_right = p->location - board_size -1;
            if (board[up_right] == board_open ||
                board[up_right] == board_player1)
                moves.push_back(Move(p->location, up_right));
        }
    }

    return moves;
}

int GameState::Evaluate(GameState &s)
{
	return Evaluate(s, s.turn);
}

int GameState::Evaluate(GameState &s, Players turn)
{
	int score = 0;
	
	Players opposite_turn;
	Board opposite_board;
	
	if(turn == player1)
	{
		opposite_turn = player2;
		opposite_board = board_player2;
	}
	else {
		opposite_turn = player1;
		opposite_board = board_player1;
	}

	vector<Move> current_moves = s.get_moves(turn);
	vector<Move> opponent_moves = s.get_moves(opposite_turn);
	
	for(int i = 0; i < current_moves.size(); i++)
	{
		int row = current_moves[i].to/10;
		
		//p1 win
		if(turn == player1 && current_moves[i].to >= 81)
		{
			score += 100000;
		}
		
		//p2 win
		if(turn == player2 && current_moves[i].to < 21)
		{
			score += 100000;
		}
		
		//capture
		if(board[current_moves[i].to] == opposite_board)
		{
			score += 100;
			
			//counter-capture
			for(int j = 0; j < opponent_moves.size(); j++)
			{
				if(opponent_moves[j].to == current_moves[i].to)
				{
					score -= 50;
				}
			}
		}
		
		//p1 next row
		if(turn == player1)
		{
			score += 2 * row;
		}
		
		//p2 next row
		if(turn == player2)
		{
			score += 2 * (row - 10);
		}
		
		//p1 support
		if(turn == player1)
		{
			if(board[current_moves[i].to + 9] == board_player1 || board[current_moves[i].to + 11] == board_player1)
			{
				score += 50;
			}
		}
		
		//p2 support
		if(turn == player2)
		{
			if(board[current_moves[i].to - 9] == board_player2 || board[current_moves[i].to - 11] == board_player2)
			{
				score += 50;
			}
		}
				
	}
	
	return score;
}

GameState& GameState::apply_move(const Move m)
{
    // Store the move on the history stack
    move_history.push_back(m);

    // Only move if an actual move
    if (!m.isNull())
    {
        // Find the pieces involved in the move
        size_t mover = pieces.size();
        size_t captured = pieces.size();
        for (size_t i = 0; i < pieces.size(); ++i)
        {
            if (pieces[i].location == m.from)
                mover = i;
            else if (pieces[i].location == m.to)
                captured = i;
        }

        // Move the piece
        board[m.from] = board_open;
        if (pieces[mover].player == player1)
            board[m.to] = board_player1;
        else
            board[m.to] = board_player2;
        pieces[mover].location = m.to;

        // Remove the captured piece if applicable
        if (captured != pieces.size())
            pieces.erase(pieces.begin() + captured);
    }

    // Change whose turn it is
    turn = (turn == player1) ? player2 : player1;

    return *this;
}

GameState& GameState::undo_move()
{
    // Not needed for server/random player so not implemented
    return *this;
}

bool GameState::game_over() const
{
    if (move_history.size() > 1 && move_history.back().isNull())
        return true;

    int p1_count = 0;
    int p2_count = 0;

    for (vector<Piece>::const_iterator p = pieces.begin(), e = pieces.end(); p != e; ++p)
    {
        // Check if piece is in last row
        if ((p->player == player1 && p->location > (board_size + 2) * board_size)
            || (p->player == player2 && p->location < 2 * (board_size + 2)))
            return true;
        if (p->player == player1) ++p1_count;
        if (p->player == player2) ++p2_count;
    }

    // Game is over if one player has no pieces
    if (p1_count == 0 || p2_count == 0)
        return true;

    return false;
}

Players GameState::get_whose_turn() const
{
    return turn;
}

string GameState::pretty_print_location(const size_t location) const
{
    char column = static_cast<char>(location % (board_size + 2)) - 1 + 'a';
    size_t row = location / (board_size + 2);
    ostringstream oss;
    oss << column << " " << row;
    return oss.str();
}

string GameState::pretty_print_move(const Move m) const
{
    return "MOVE " + pretty_print_location(m.from) + " TO " + pretty_print_location(m.to);
}


void GameState::reset()
{
    // Sets the board up for a new game
    board.clear();
    board.reserve((board_size + 2) * (board_size + 2));

    // Make the board open except for the borders
    for (size_t i = 0; i < board_size + 2; ++i)
        for (size_t j = 0; j < board_size + 2; ++j)
            if (i == 0 || i == (board_size + 1) || j == 0 || j == (board_size + 1))
                board.push_back(board_closed);
            else
                board.push_back(board_open);


    pieces.clear();
    pieces.reserve(4 * board_size);
    // Player 1 pieces
    for (size_t i = board_size + 3; i < 3 * (board_size + 2); ++i)
        if (i % (board_size + 2) !=  0 && i % (board_size + 2) != board_size + 1)
        {
            pieces.push_back(Piece(i, player1));
            board[i] = board_player1;
        }

    // Player 2 pieces
    for (size_t i = (board_size + 2) * (board_size - 1);
         i < (board_size + 2) * (board_size + 1); ++i)
        if (i % (board_size + 2) !=  0 && i % (board_size + 2) != board_size + 1)
        {
            pieces.push_back(Piece(i, player2));
            board[i] = board_player2;
        }

    // Everything else
    turn = player1;
}

Move GameState::translate_to_local(const vector<string> message) const
{
    size_t from = 0;
    size_t to = 0;

    if (message[0] == "MOVE")
    {
        // Message source is a client
        from = message[1].at(0) - 'a' + 1 + String::stoi(message[2]) * (board_size + 2);
        to = message[4].at(0) - 'a' + 1 + String::stoi(message[5]) * (board_size + 2);
    }
    else
    {
        // Message source is the server
        from = message[2].at(0) - 'a' + 1 + String::stoi(message[3]) * (board_size + 2);
        to = message[5].at(0) - 'a' + 1 + String::stoi(message[6]) * (board_size + 2);
    }

    return Move(from, to);
}

bool GameState::valid_move(const Move m) const
{
    return valid_move(m, turn);
}

bool GameState::valid_move(const Move m, const Players player) const
{
    // Can't make a move if game is over
    if (game_over())
        return false;

    // Validate the locations are in plausible ranges
    // Only need to check upper bounds because location type is unsigned
    if (m.from > (board_size + 2) * (board_size + 2)
        || m.to > (board_size + 2) * (board_size + 2))
    {
        cerr << "Move " << m.from << " to " << m.to << " is outside plausible board range" << endl;
        return false;
    }

    // Find the pieces involved in the move
    size_t mover = pieces.size();
    for (size_t i = 0; i < pieces.size(); ++i)
    {
        if (pieces[i].location == m.from)
        {
            mover = i;
            break;
        }
    }

    // There needs to be a piece at the from location
    if (mover == pieces.size())
    {
        cerr << "Move has no piece at from location" << endl;
        return false;
    }

    // The moved piece needs to be owned by the current player
    if (pieces[mover].player != player)
    {
        cerr << "Move trying to move opponents piece" << endl;
        return false;
    }

    // The to location either needs to be open, or occupied by an opposing piece
    if (board[m.to] == board_closed)
    {
        cerr << "Move results in piece being off the board" << endl;
        return false;
    }
    if ((board[m.from] == board_player1 && board[m.to] == board_player1)
        || (board[m.from] == board_player2 && board[m.to] == board_player2))
    {
        cerr << "Move tries to capture own piece" << endl;
        return false;
    }

    // The move needs to be a valid for the current player
    size_t diff = 0;
    if (player == player1)
        diff = m.to - m.from;
    else
        diff = m.from - m.to;

    // If up or down, can't capture
    if (diff == board_size + 2 && board[m.to] != board_open)
    {
        cerr << "Move tries to capture, but it is not a diagonal move" << endl;
        return false;
    }

    if (board_size + 1 <= diff && diff <= board_size +3)
        return true;

    // Not a valid move
    return false;
}

int GameState::ModifyDepth(int depth, int PossibleMoves)
{
	if(PossibleMoves < 9)
		depth = (depth + 2);
	return depth;
}

std::vector<GameState> GameState::PossibleBoards(GameState& s, Players turn)
{
	vector<GameState> successors;
	vector<Move> moves = get_moves(turn);
	
	for(int i = 0; i < moves.size(); i++)
	{
		apply_move(moves[i]);
		successors.push_back(*this);
		undo_move();
	}
	
	return successors;
}

Move GameState::RunMove(GameState &s, int MaxDepth, Players turn)
{
	return FixedDepthAlphaBetaMove(s, MaxDepth, turn);
}

void GameState::FixedDepthAlphaBetaVoid(GameState &s, int MaxDepth, Players turn)
{
	AlphaBetaTTRootVoid(s, MaxDepth, turn, true);
}

GameState& GameState::FixedDepthAlphaBeta(GameState &s, int MaxDepth, Players turn)
{
	return AlphaBetaTTRoot(s, MaxDepth, turn, true);
}

Move GameState::FixedDepthAlphaBetaMove(GameState &s, int MaxDepth, Players turn)
{
	return AlphaBetaTTRootMove(s, MaxDepth, turn, true);
}

bool GameState::SortSuccessors(GameState i, GameState j)
{
	return (Evaluate(i) > Evaluate(j));
}

void GameState::AlphaBetaTTRootVoid(GameState &s, int Depth, Players turn, bool AllowNullMove)
{
	// Perform variable initializations
	int best_value = -Infinity;
	bool FirstCall = true;
	NodesSearchedLast = 0;
	Depth--;
	
	// Get all possible boards
	std::vector<GameState> Successors = PossibleBoards(s, turn);
	int total_boards = Successors.size(); // count
	
	// Evaluate all and order the root
	//std::sort(Successors.begin(), Successors.end(), SortSuccessors);
	
	// Result value initialization
	GameState best_board;
	best_board = Successors[0];
	const std::vector<Move> move_vector = get_moves(turn);
	Move best_current_move = move_vector[0]; 
	
	for (int i = 0; i < total_boards; i++)
	{
		GameState current = Successors[i];
		int value;
		Players opposite_turn;
		
		if(turn == player1)
		{
			opposite_turn = player2;
		}
		else {
			opposite_turn = player1;
		}
		
		
		if (FirstCall)
		{ // First call, alpha = -infinity and beta = +infinity
			value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -Infinity, -best_value, opposite_turn, AllowNullMove);
		}
		else
		{ // Better value found
			value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -best_value - 1, -best_value, opposite_turn, AllowNullMove);
			if (value > best_value)
			{
				value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -Infinity, -best_value, opposite_turn, AllowNullMove);
			}
		}
		
		if (value > best_value) // value is better
		{
			best_value = value;
			
			best_board = current;
			best_current_move = move_vector[i];
			//best_current_move = SuccessorMoves[i];
			//BestBoard.Value = Value;
			FirstCall = false;
		}
		
	}
	
	
	//	if (Watch.ElapsedMilliseconds >= TimeToMoveMs)
	//		return null; // no more time
	//my_move = best_current_move;
}

GameState& GameState::AlphaBetaTTRoot(GameState &s, int Depth, Players turn, bool AllowNullMove)
{
	// Perform variable initializations
	int best_value = -Infinity;
	bool FirstCall = true;
	NodesSearchedLast = 0;
	Depth--;
	
	// Get all possible boards
	std::vector<GameState> Successors;
	std::vector<Move> all_moves = get_moves(turn);
	
	for(int i = 0; i < all_moves.size(); i++)
	{
		Successors.push_back(apply_move(all_moves[i]));
	}
	
	int total_boards = Successors.size(); // count
	
	// Evaluate all and order the root
	//std::sort(Successors.begin(), Successors.end(), SortSuccessors);
	
	// Result value initialization
	GameState best_board;
	best_board = Successors[0];
	const std::vector<Move> move_vector = get_moves(turn);
	Move best_current_move = move_vector[0]; 
	
	for (int i = 0; i < total_boards; i++)
	{
		GameState current = Successors[i];
		int value;
		Players opposite_turn;
		
		if(turn == player1)
		{
			opposite_turn = player2;
		}
		else {
			opposite_turn = player1;
		}

		
		if (FirstCall)
		{ // First call, alpha = -infinity and beta = +infinity
			value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -Infinity, -best_value, opposite_turn, AllowNullMove);
		}
		else
		{ // Better value found
			value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -best_value - 1, -best_value, opposite_turn, AllowNullMove);
			if (value > best_value)
			{
				value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -Infinity, -best_value, opposite_turn, AllowNullMove);
			}
		}
		
		if (value > best_value) // value is better
		{
			best_value = value;
			
			best_board = current;
			best_current_move = move_vector[i];
			//best_current_move = SuccessorMoves[i];
			//BestBoard.Value = Value;
			FirstCall = false;
		}
		
	}
	
	
	//	if (Watch.ElapsedMilliseconds >= TimeToMoveMs)
	//		return null; // no more time
	//myMove = best_current_move;
	return best_board;
}

Move GameState::AlphaBetaTTRootMove(GameState &s, int Depth, Players turn, bool AllowNullMove)
{
	// Perform variable initializations
	int best_value = -Infinity;
	bool FirstCall = true;
	NodesSearchedLast = 0;
	Depth--;
	size_t from = 0;
    size_t to = 0;
	
	// Get all possible boards
	std::vector<GameState> Successors = PossibleBoards(s, turn);
	int total_boards = Successors.size(); // count
	
	// Evaluate all and order the root
	//std::sort(Successors.begin(), Successors.end(), SortSuccessors);
	
	// Result value initialization
	GameState best_board;
	best_board = Successors[0];
	const std::vector<Move> move_vector = get_moves(turn);
	Move best_current_move(move_vector[0].from, move_vector[0].to); 
	
	for (int i = 0; i < total_boards; i++)
	{
		GameState current = Successors[i];
		int value;
		Players opposite_turn;
		
		if(turn == player1)
		{
			opposite_turn = player2;
		}
		else {
			opposite_turn = player1;
		}
		
		
		if (FirstCall)
		{ // First call, alpha = -infinity and beta = +infinity
			value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -Infinity, -best_value, opposite_turn, AllowNullMove);
		}
		else
		{ // Better value found
			value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -best_value - 1, -best_value, opposite_turn, AllowNullMove);
			if (value > best_value)
			{
				value = -AlphaBetaTT(current, ModifyDepth(Depth, Successors.size()), -Infinity, -best_value, opposite_turn, AllowNullMove);
			}
		}
		
		if (value > best_value) // value is better
		{
			best_value = value;
			
			best_board = current;
			from = move_vector[i].from;			
			to = move_vector[i].to;
			best_current_move.from = from;
			best_current_move.to = to;
			//best_current_move = SuccessorMoves[i];
			//BestBoard.Value = Value;
			FirstCall = false;
		}
		
	}
	
	
	//	if (Watch.ElapsedMilliseconds >= TimeToMoveMs)
	//		return null; // no more time
	
	return best_current_move;
}

int GameState::AlphaBetaTT(GameState &ExamineBoard, int Depth, int Alpha, int Beta, Players turn, bool AllowNullMove)
{
	// Check time
	double duration = myTimer->TimeElapsed(*myTimer);
	
	if (duration >= MaxMoveTime)
	{
		//std::cout << "AlphaBetaTT() has timed out" << std::endl;
		return 0; // timeout
	}
	
	if (Depth == 0 || ExamineBoard.game_over())
	{
		int value = Evaluate(ExamineBoard, turn) + Depth; // add depth (since it's inverse)
		return value;
		
	}
	
	NodesSearchedLast++;
	
	//Apply null move restrictions
	if (Depth >= 2 && Beta < Infinity && AllowNullMove && ExamineBoard.get_num_pieces_total() > 15)
	{
		// Try null move
		int r = 1;
		if (Depth >= 4) r = 2;
		else if (Depth >= 7) r = 3;
		
		Move nullMove(0, 0);
		//nullMove.from = 0;
		//nullMove.to = 0;
		
		ExamineBoard.apply_move(nullMove);
		int value = -AlphaBetaTT(ExamineBoard, (Depth - r - 1), -Beta, -Beta + 1, turn, false);
		ExamineBoard.undo_move();
		
		if (value >= Beta)
		{
			return value;
		}
	}
	
	
	std::vector<GameState> Successors = PossibleBoards(ExamineBoard, turn);
	int totalBoards = Successors.size();
	
	Players opposite_turn;
	if(turn == player1)
	{
		opposite_turn = player2;
	}
	else {
		opposite_turn = player1;
	}

	
	if (totalBoards == 0)
		return Evaluate(ExamineBoard);
	
	// sort the boards in order to have better pruning
	//std::sort(Successors.begin(), Successors.end(), SortSuccessors);
	Depth--;
	
	int Best = -WinValue - 1;
	
	GameState BoardToEvaluate;
	for (int i = 0; i < totalBoards; i++)
	{
		BoardToEvaluate = Successors[i];
		int value = -AlphaBetaTT(BoardToEvaluate, Depth, -Beta, -Alpha, opposite_turn, true);
		
		if (value > Best)
			Best = value;
		if (Best > Alpha)
			Alpha = Best;
		if (Best >= Beta)
			break;
	}
	
	return Best;
}



