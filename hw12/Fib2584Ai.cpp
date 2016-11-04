#include "Fib2584Ai.h"


Fib2584Ai::Board::MoveRow *Fib2584Ai::Board::moverow;
std::unordered_map<int, int> Fib2584Ai::Board::fibonacci_to_int;

Fib2584Ai::Fib2584Ai() {
    if(Board::fibonacci_to_int.size() == 0){
        int fibonacci_[32] = {0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309};
        for(int i = 0 ; i < 32 ; i++)
            Board::fibonacci_to_int[fibonacci_[i]] = i;
        Board::moverow = new Board::MoveRow[0x100000];
        for(int i = 0 ; i < 0x100000 ; i++){
            Board::moverow[i].calc(i);
        }
    }
}

void Fib2584Ai::initialize(int argc, char* argv[]) {
	srand(time(NULL));
    if( argc >=  3 ){
        HeartBeat = atoi(argv[2]);
    } else {
        HeartBeat = false;
    }
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4]) {
    /*
    if( HeartBeat )
        std::cout << "." << std::flush;
        */
    return td.generateMove(board);
}

void Fib2584Ai::gameOver(int board[4][4], int iScore) {
    if( HeartBeat )
        std::cout << "." << std::flush;
        //std::cout << std::endl;
    td.gameOver(board);
	return;
}


MoveDirection Fib2584Ai::MinMax::generateMove(int board[4][4]) {
    Board now(board);
    MinMaxResult res = Node(now, 5);
    if(res.first == 0) {
        return static_cast<MoveDirection>(rand() % 4);
    }
    int zero = 0;
    for(int i = 0 ; i < 4 ; i++)
        for(int j = 0 ; j < 4 ; j++)
            if(now.getTile(i, j) == 0) zero++;
    int step = std::max(5, 9 - (zero / 4) * 2);
    if(step == 5)
        return static_cast<MoveDirection>(res.second);
    MoveDirection next_step = static_cast<MoveDirection>(Node(now, step).second);
    return next_step;
}

Fib2584Ai::MinMax::MinMaxResult Fib2584Ai::MinMax::Node(Board board, int level){
    if(level == 0){
        return MinMaxResult(0, 0);
    }
    MinMaxResult res;
    if(level & 1){
        for(int i = 0 ; i < 4 ; i++){
            Board next_board;
            int score;
            next_board = board;
            next_board.move(i, score);
            if(next_board == board) continue;
            score += Node(next_board, level - 1).first;
            if(score > res.first){
                res.first = score;
            }
            if(score == res.first)
                res.second = i;
        }
    } else {
        res.first = 1029384756;
        for(int i = 0 ; i < 4 ; i++){
            for(int j = 0 ; j < 4 ; j++){
                Board next_board;
                next_board = board;
                if(next_board.getTile(i, j) != 0) continue;
                int K[2] = {1, 3};
                for(int k = 0 ; k < 2 ; k++)
                    next_board.setTile(i, j, K[k]);
                res.first = std::min(res.first, Node(next_board, level-1).first);
            }
        }
    }
    return res;
}

int Fib2584Ai::TD::feature_type;
int Fib2584Ai::TD::weight_type;
std::vector< int > Fib2584Ai::TD::FeatureMapping;



Fib2584Ai::TD::TD(bool _train, const std::string _filename){
    train = _train;
    train = true;
    filename = _filename;
    weight_type = 6;
    weight = new float* [weight_type]();
    feature_type = 48;
    int feature_num = 22 * 22 * 22 * 22 * 22 * 22;
    int fd = open(filename.c_str(), O_RDONLY);
    for(int i = 0 ; i < weight_type ; i++){
        weight[i] = new float[feature_num];
        int re = read(fd, weight[i], feature_num * sizeof(float));
    }
    close(fd);
    for(int i = 0 ; i < feature_type ; i++){
        FeatureMapping.push_back(i % weight_type);
    }
}

Fib2584Ai::TD::~TD(){
    if(train){
        int fd = open(filename.c_str(), O_WRONLY | O_CREAT, 0644);
        int feature_num = 22 * 22 * 22 * 22 * 22 * 22;
        for(int i = 0 ; i < weight_type ; i++){
            int re = write(fd, weight[i], feature_num * sizeof(float));
        }
    }
    for(int i = 0 ; i < weight_type ; i++)
        delete [] weight[i];
    delete [] weight;
}

MoveDirection Fib2584Ai::TD::generateMove(int board[4][4]){
    const double random_rate = 0;
    Board now(board);
    MoveDirection dir;
    double heighest = -1029384756;
    Feature bestFeature;
    if( double(rand()) / RAND_MAX < random_rate ) {
        dir = (MoveDirection)(rand()%4);
        Board next;
        next = now;
        int score;
        next.move(dir, score);
        bestFeature = Feature(next, score);
    } else {
        for(int direction = 0 ; direction < 4 ; direction++){
            Board next;
            next = now;
            int score;
            next.move(direction, score);
            if(now == next) continue;
            Feature nextFeature(next, score);
            double valuePlusScore = getValue(nextFeature) + score;
            if(heighest < valuePlusScore){
                dir = (MoveDirection)direction;
                heighest = valuePlusScore;
                bestFeature = nextFeature;
            }
        }
    }
    if(train){
        history.push(bestFeature);
    }
    return dir;
}

void Fib2584Ai::TD::gameOver(int board[4][4]){
    const double alpha = 1e-5;
    if(train){
        Feature nextFeature, feature;
        double delta;
        feature = Feature(Board(board), 0);
        delta = -getValue(feature);
        for(int i = 0 ; i < feature_type ; i++){
            weight[FeatureMapping[i]][feature.feature[i]] += alpha * delta;
        }
        nextFeature = feature;
        while(history.size()){
            feature = history.top();
            history.pop();
            delta = getValue(nextFeature) + nextFeature.reward - getValue(feature);
            for(int i = 0 ; i < feature_type ; i++){
                weight[FeatureMapping[i]][feature.feature[i]] += ((alpha * delta));
            }
            nextFeature = feature;
        }
    }
}

Fib2584Ai::TD::Feature::Feature(){
}

Fib2584Ai::TD::Feature::Feature(Board board, int _reward){
    int index = 0;
    Board src = board;
    for(int k = 0 ; k < 2 ; k++){
        board = src;
        if(k) board.reflex(1);
        for(int i = 0 ; i < 4 ; i++){
            //feature[index++] = rehash((board.getRow(1) & 0x7fff) | ((board.getRow(2) & 0x7fff) << 15));
            feature[index++] = rehash((board.getRow(0) & 0x3ff) | (board.getRow(1) << 10));
            feature[index++] = rehash((board.getRow(0)) | ((board.getRow(1) & 0x3ff)<<20));
            feature[index++] = rehash((board.getRow(1)) | ((board.getRow(2) & 0x3ff)<<20));
            feature[index++] = rehash(((board.getRow(0)>>5) & 0x3ff) | (board.getRow(1) << 10));
            feature[index++] = rehash((board.getRow(0)) | (((board.getRow(1) >> 5) & 0x3ff)<<20));
            feature[index++] = rehash((board.getRow(1)) | (((board.getRow(2) >> 5) & 0x3ff)<<20));
            board.rotate();
        }
    }
    reward = _reward;
}

int Fib2584Ai::TD::Feature::rehash(int x){
    int re = 0;
    for(int i = 0 ; i < 6 ; i++){
        re = re * 22 + (( x >> (i * 5 ) ) & 0x1f);
    }
    return re;
}

Fib2584Ai::TD::Feature::Feature(const Feature &_feature){
    reward = _feature.reward;
    for(int i = 0 ; i < feature_type ; i++){
        feature[i] = _feature.feature[i];
    }
}

double Fib2584Ai::TD::getValue(Fib2584Ai::TD::Feature feature){
    double value = 0;
    for(int i = 0 ; i < feature_type ; i++){
        value += weight[FeatureMapping[i]][feature.feature[i]];
    }
    return value;
}

Fib2584Ai::Board::MoveRow::MoveRow(){
}

void Fib2584Ai::Board::MoveRow::calc(__int128 value){
    now_row = value;
    int val[4];
    int fibonacci_[32] = {0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309};
    for(int i = 0 ; i < 2 ; i++){
        moveable[i] = false;
        score[i] = 0;
        for(int j = 0 ; j < 4 ; j++)
            val[j] = (value >> (5 * j)) & 0x1f;
        if(i){
            std::swap(val[0], val[3]);
            std::swap(val[1], val[2]);
        }
        for(int u = 0 ; u < 4 ; u++){
            if(val[u] == 0){
                for(int v = u + 1 ; v < 4 ; v++){
                    if(val[v] != 0){
                        std::swap(val[u], val[v]);
                        moveable[i] = true;
                        break;
                    }
                }
            }
        }
        for(int u = 0 ; u < 3 ; u++){
            if(val[u] && val[u+1] && (abs(val[u] - val[u+1]) == 1 || (val[u] == 1 && val[u+1] == 1))){
                moveable[i] = true;
                val[u] = std::max(val[u], val[u+1]) + 1;
                val[u+1] = 0;
                score[i] += fibonacci_[val[u]];
            }
        }
        for(int u = 0 ; u < 4 ; u++){
            if(val[u] == 0){
                for(int v = u + 1 ; v < 4 ; v++){
                    if(val[v] != 0){
                        std::swap(val[u], val[v]);
                        moveable[i] = true;
                        break;
                    }
                }
            }
        }
        if(i){
            std::swap(val[0], val[3]);
            std::swap(val[1], val[2]);
        }
        for(int u = 0 ; u < 4 ; u++){
            next_row[i] |= __int128(val[u]) << (5 * u);
        }
    }
}

Fib2584Ai::Board::Board(){
    _board = 0;
}

Fib2584Ai::Board::Board(int board[4][4]){
    _board = 0;
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4 ; j++){
            setTile(i, j, fibonacci_to_int[board[i][j]]);
        }
    }
}

void Fib2584Ai::Board::show(){
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4 ; j++)
            std::cout << getTile(i, j) << ' ';
        std::cout << std::endl;
    }
}

void Fib2584Ai::Board::setTile(int x, int y, __int128 value){
    _board |= value << hashpos(x, y);
}

int Fib2584Ai::Board::getTile(int x, int y){
    return (_board >> hashpos(x, y)) & 31;
}

int Fib2584Ai::Board::hashpos(int x, int y){
    return (5 * (x * 4 + y));
}

__int128 Fib2584Ai::Board::getRow(int x){
    __int128 re = 0;
    for(int i = 0 ; i < 4 ; i++){
        re |= getTile(x, i) << (5 * i);
    }
    return re;
}

__int128 Fib2584Ai::Board::getColumn(int x){
    __int128 re = 0;
    for(int i = 0 ; i < 4 ; i++){
        re |= getTile(i, x) << (5 * i);
    }
    return re;
}

void Fib2584Ai::Board::setRow(int x, __int128 value){
    for(int i = 0 ; i < 4 ; i++){
        setTile(x, i, (value >> (5 * i)) & 0x1f);
    }
}

void Fib2584Ai::Board::setColumn(int x, __int128 value){
    for(int i = 0 ; i < 4 ; i++){
        setTile(i, x, (value >> (5 * i)) & 0x1f);
    }
}

void Fib2584Ai::Board::move(int direction, int &score){
    Board new_board;
    score = 0;
    if(direction == 0 || direction == 2){
        for(int i = 0 ; i < 4 ; i++){
            __int128 tmp = getColumn(i);
            new_board.setColumn( i, moverow[tmp].next_row[direction == 2] );
            score += moverow[tmp].score[direction == 2];
        }
    } else {
        for(int i = 0 ; i < 4 ; i++){
            __int128 tmp = getRow(i);
            new_board.setRow( i, moverow[tmp].next_row[direction == 1] );
            score += moverow[tmp].score[direction == 1];
        }
    }
    _board = new_board._board;
}

void Fib2584Ai::Board::rotate(){
    Board new_board;
    for(int i = 0 ; i < 4 ; i++)
        for(int j = 0 ; j < 4 ; j++)
            new_board.setTile(i, j, getTile(j, i));
    new_board.reflex(0);
    _board = new_board._board;
}

void Fib2584Ai::Board::reflex(int type){
    Board new_board;
    if(type == 0){
        for(int i = 0 ; i < 4 ; i++){
            new_board.setRow(i, getRow(4 - i - 1));
        }
    } else {
        for(int i = 0 ; i < 4 ; i++){
            new_board.setColumn(i, getColumn(4 - i - 1));
        }
    }
    _board = new_board._board;
}

bool Fib2584Ai::Board::operator==(const Board &b){
    return _board == b._board;
}

