#include "Fib2x3Solver.h"

Fib2x3Solver::Fib2x3Solver(){
}

int Fib2x3Solver::fib[32];
std::unordered_map<int, int> Fib2x3Solver::fib2n;
std::unordered_map<int, std::pair<int, double> > Fib2x3Solver::ans;

void Fib2x3Solver::initialize(int argc, char* argv[]){
	auto x = clock();
	fib[1] = 1;
	fib[2] = 2;
	for(int i = 3 ; i < 32 ; i++){
		fib[i] = fib[i-1] + fib[i-2];
	}
	for(int i = 0 ; i < 32 ; i++)
		fib2n[fib[i]] = i;
	for(int i = 0 ; i < FIB_ROW * FIB_COLUMN ; i++){
		for(int j = 0 ; j < FIB_ROW * FIB_COLUMN ; j++){
			if(i == j) continue;
			for(auto x : {1, 3})
				for(auto y : {1, 3}){
					Board board;
					board.setTile(i / FIB_COLUMN, i % FIB_COLUMN, x);
					board.setTile(j / FIB_COLUMN, j % FIB_COLUMN, y);
					evaluteExpectedScore(board);
				}
		}
	}
	printf("%lu\n", ans.size());
	printf("Build the %.f ms\n", double(clock() - x) / double(CLOCKS_PER_SEC) * 1000);
}

double Fib2x3Solver::evaluteExpectedScore(Board board){
	if(ans.find(board.board) != ans.end())
		return ans[board.board].second;
	int best_direction = 4;
	double max_score = 0;
	for(int dir = 0 ; dir < 4 ; dir++){
		double score = 0;
		int reward = 0;
		Board moved = board;
		moved.move(dir, reward);
		if(moved == board) continue;
		int count = 0;
		for(int i = 0 ; i < FIB_ROW ; i++){
			for(int j = 0 ; j < FIB_COLUMN ; j++){
				if(moved.getTile(i, j) == 0){
					count++;
					Board next = moved;
					next.setTile(i, j, 1);
					score += 0.75 * evaluteExpectedScore(next);
					next = moved;
					next.setTile(i, j, 3);
					score += 0.25 * evaluteExpectedScore(next);
				}
			}
		}
		if( score / count + reward > max_score){
			best_direction = dir;
			max_score = score / count + reward;
		}
	}
	ans[board.board].second = max_score;
	ans[board.board].first = best_direction;
	return max_score;
}

double Fib2x3Solver::evaluteExpectedScore(int arrayboard[FIB_ROW][FIB_COLUMN]){
	if(ans.find(Board(arrayboard).board) == ans.end())
		return -1;
	return ans[Board(arrayboard).board].second;
}

int Fib2x3Solver::findBestMove(int arrayboard[FIB_ROW][FIB_COLUMN]){
	if(ans.find(Board(arrayboard).board) == ans.end())
		return 4;
	return ans[Board(arrayboard).board].first;
}

Fib2x3Solver::Board::Board(){
	board = 0;
}

Fib2x3Solver::Board::Board(const Board& rhs){
	board = rhs.board;
}

Fib2x3Solver::Board::Board(int _board[FIB_ROW][FIB_COLUMN]){
	board = 0;
	for(int i = 0 ; i < FIB_ROW ; i++)
		for(int j = 0 ; j < FIB_COLUMN ; j++)
			setTile(i, j, fib2n[_board[i][j]]);
}

void Fib2x3Solver::Board::setTile(int x, int y, int value){
	board &= 0xffffffff ^ (0x1f << ((x * FIB_COLUMN + y) * 5));
	board |= value << ((x * FIB_COLUMN + y) * 5);
}

int Fib2x3Solver::Board::getTile(int x, int y){
	return (board >> ((x * FIB_COLUMN + y) * 5)) & 0x1f;
}

void Fib2x3Solver::Board::setRow(int x, int value){
	for(int i = 0 ; i < FIB_COLUMN ; i++)
		setTile(x, i, (value >> (i * 5)) & 0x1f);
}

int Fib2x3Solver::Board::getRow(int x){
	int re = 0;
	for(int i = 0 ; i < FIB_COLUMN ; i++)
		re |= getTile(x, i) << (5 * i);
	return re;
}

void Fib2x3Solver::Board::setColumn(int x, int value){
	for(int i = 0 ; i < FIB_ROW ; i++)
		setTile(i, x, (value >> (i * 5)) & 0x1f);
}

int Fib2x3Solver::Board::getColumn(int x){
	int re = 0;
	for(int i = 0 ; i < FIB_ROW ; i++)
		re |= getTile(i, x) << (5 * i);
	return re;
}

void Fib2x3Solver::Board::reverseColumn(){
	Board new_board;
	for(int i = 0 ; i < FIB_COLUMN ; i++)
		new_board.setColumn(FIB_COLUMN - i - 1, getColumn(i));
	(*this) = new_board;
}

void Fib2x3Solver::Board::reverseRow(){
	Board new_board;
	for(int i = 0 ; i < FIB_ROW ; i++)
		new_board.setRow(FIB_ROW - i - 1, getRow(i));
	(*this) = new_board;
}

void Fib2x3Solver::Board::move(int direction, int &score){
	if(direction == 0)
		moveUp(score);
	else if(direction == 1)
		moveRight(score);
	else if(direction == 2)
		moveDown(score);
	else if(direction == 3)
		moveLeft(score);
}

void Fib2x3Solver::Board::moveDown(int &score){
	reverseRow();
	moveUp(score);
	reverseRow();
}

void Fib2x3Solver::Board::moveUp(int &score){
	score = 0;
	for(int k = 0 ; k < FIB_ROW - 1 ; k++){
		for(int i = 1 ; i < FIB_ROW ; i++){
			for(int j = 0 ; j < FIB_COLUMN ; j++){
				if(getTile(i - 1, j) == 0){
					int x = getTile(i - 1, j);
					int y = getTile(i, j);
					setTile(i - 1, j, y);
					setTile(i, j, x);
				}
			}
		}
	}
	for(int i = 0 ; i < FIB_ROW - 1 ; i++){
		for(int j = 0 ; j < FIB_COLUMN ; j++){
			if( (getTile(i, j) && getTile(i+1, j) && abs(getTile(i, j) - getTile(i+1, j)) == 1) ||
				(getTile(i, j) == getTile(i+1, j) && getTile(i, j) == 1)){
				setTile(i, j, std::max(getTile(i, j), getTile(i+1, j)) + 1);
				setTile(i+1, j, 0);
				score += fib[getTile(i, j)];
			}
		}
	}
	for(int k = 0 ; k < FIB_ROW - 1 ; k++){
		for(int i = 1 ; i < FIB_ROW ; i++){
			for(int j = 0 ; j < FIB_COLUMN ; j++){
				if(getTile(i - 1, j) == 0){
					int x = getTile(i - 1, j);
					int y = getTile(i, j);
					setTile(i - 1, j, y);
					setTile(i, j, x);
				}
			}
		}
	}
}

void Fib2x3Solver::Board::moveLeft(int &score){
	score = 0;
	for(int k = 0 ; k < FIB_COLUMN - 1 ; k++){
		for(int i = 1 ; i < FIB_COLUMN ; i++){
			for(int j = 0 ; j < FIB_ROW ; j++){
				if(getTile(j, i - 1) == 0){
					int x = getTile(j, i - 1);
					int y = getTile(j, i);
					setTile(j, i - 1, y);
					setTile(j, i, x);
				}
			}
		}
	}
	for(int i = 0 ; i < FIB_COLUMN - 1 ; i++){
		for(int j = 0 ; j < FIB_ROW ; j++){
			if( (getTile(j, i) && getTile(j, i+1) && abs(getTile(j, i) - getTile(j, i+1)) == 1) ||
				(getTile(j, i) == getTile(j, i+1) && getTile(j, i) == 1)){
				setTile(j, i, std::max(getTile(j, i), getTile(j, i+1)) + 1);
				setTile(j, i+1, 0);
				score += fib[getTile(j, i)];
			}
		}
	}
	for(int k = 0 ; k < FIB_COLUMN - 1 ; k++){
		for(int i = 1 ; i < FIB_COLUMN ; i++){
			for(int j = 0 ; j < FIB_ROW ; j++){
				if(getTile(j, i - 1) == 0){
					int x = getTile(j, i - 1);
					int y = getTile(j, i);
					setTile(j, i - 1, y);
					setTile(j, i, x);
				}
			}
		}
	}
}

void Fib2x3Solver::Board::moveRight(int &score){
	reverseColumn();
	moveLeft(score);
	reverseColumn();
}

void Fib2x3Solver::Board::print(){
	for(int i = 0 ; i < FIB_ROW ; i++){
		for(int j = 0 ; j < FIB_COLUMN ; j++){
			printf("%d ", getTile(i, j));
		}
		printf("\n");
	}
}

Fib2x3Solver::Board Fib2x3Solver::Board::operator=(const Board& rhs){
	board = rhs.board;
	return (*this);
}

bool Fib2x3Solver::Board::operator==(const Board& rhs) const {
	return board == rhs.board;
}

bool Fib2x3Solver::Board::operator!=(const Board& rhs) const {
	return !( (*this) == rhs );
}
