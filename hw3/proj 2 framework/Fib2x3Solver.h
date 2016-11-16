#ifndef __FIB2X3SOLVER_H__
#define __FIB2X3SOLVER_H__
#include <unordered_map>
#include <cstdlib>
#include <algorithm>
// include anything you might use

#define FIB_ROW 2
#define FIB_COLUMN 3

class Fib2x3Solver
{
private:
	struct Board;
public:
	Fib2x3Solver();
	void initialize(int argc, char* argv[]);
	double evaluteExpectedScore(Board board);
	double evaluteExpectedScore(int board[FIB_ROW][FIB_COLUMN]);
	int findBestMove(int board[FIB_ROW][FIB_COLUMN]);
	static int fib[32];
	static std::unordered_map<int, int> fib2n;
	static std::unordered_map<int, std::pair<int, double> > ans;
private:
	struct Board{
		int board;
		Board();
		Board(const Board& rhs);
		Board(int board[FIB_ROW][FIB_COLUMN]);
		void setTile(int x, int y, int value);
		int getTile(int x, int y);
		void setRow(int x, int value);
		int getRow(int x);
		void setColumn(int x, int value);
		int getColumn(int x);
		void reverseColumn();
		void reverseRow();
		void move(int direction, int &score);
		void moveDown(int &score);
		void moveUp(int &score);
		void moveLeft(int &score);
		void moveRight(int &score);
		void print();
		Board operator=(const Board& rhs);
		bool operator==(const Board& rhs) const ;
		bool operator!=(const Board& rhs) const ;
	};
};
#endif
