#ifndef __FIB2584AI_H__
#define __FIB2584AI_H__

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <stack>
#include <cmath>
#include <unistd.h>
#include <cstdio>
#include <algorithm>
#include <fcntl.h>
#include "Fib2584/MoveDirection.h"

class Fib2584Ai{
public:
    class Board{
        public:
            __int128 _board;
            Board();
            Board(int board[4][4]);
            void show();
            void setTile(int x, int y, __int128 value);
            int getTile(int x, int y);
            int hashpos(int x, int y);
            __int128 getRow(int x);
            __int128 getColumn(int x);
            void setRow(int x, __int128 value);
            void setColumn(int x, __int128 value);
            void move(int direction, int &score);
            void rotate();
            void reflex(int type);
            bool operator==(const Board &b);

            class MoveRow {
                public:
                    __int128 now_row;
                    __int128 next_row[2];
                    bool moveable[2];
                    int score[2];
                    MoveRow();
                    void calc(__int128 value);
            };

            static MoveRow *moverow;
            static std::unordered_map<int, int> fibonacci_to_int;
    };
    class MinMax {
        public:
            MoveDirection generateMove(int board[4][4]);
        private:
            typedef std::pair<int, int> MinMaxResult;
            MinMaxResult Node(Board board, int level);
    };
    class TD {
        public:
            TD(bool _train=true, const std::string _filename=std::string("data"));
            ~TD();
            MoveDirection generateMove(int board[4][4]);
            void gameOver(int board[4][4]);
        private:
            typedef std::pair<int, double> PID;
            static int weight_type;
            static int feature_type;
            float **weight;
            std::string filename;
            bool train;
            static std::vector< int > FeatureMapping;
            class Feature{
                public:
                    Feature();
                    Feature(const Feature& _feature);
                    Feature(Board board, int _reward);
                    int rehash(int x);

                    int feature[48];
                    int reward;

            };
            double getValue(Feature feature);
            std::stack<Feature> history;
    };
public:
	Fib2584Ai();
	void initialize(int argc, char* argv[]);
	MoveDirection generateMove(int board[4][4]);
	void gameOver(int board[4][4], int iScore);
    TD td;
private:
    bool HeartBeat;
};
#endif
