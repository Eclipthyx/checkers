#include <string>
#include <iostream>
#include <vector>
#include <memory>

//Convert number string to a number
template <class X>
X		atod(std::string str){
	X c;
	X s;
	X res;

	c = 0;
	s = 1;
	res = 0;
	while ((str[c] >= '\t' && str[c] <= '\r') || str[c] == ' ')
		c++;
	while (str[c] == '+' || str[c] == '-'){
		if (str[c] == '-')
			s *= -1;
		c++;
	}
	while (str[c] >= '0' && str[c] <= '9'){
		res = (str[c] - '0') + (res * 10);
		c++;
	}
	return (res * s);
}
///////////////////////////////////////////////////////////////////////////////
class Piece {
    public:
    Piece(int player = 0, int category = 0, int y = 0, int x = 0);

    private:
    int y;
    int x;
    int player;
    int category;
    friend class Board;
};
///////////////////////////////////////
Piece::Piece(int player, int category, int y, int x) {
    this->player = player;
    this->category = category;
    this->y = y;
    this->x = x;
}
///////////////////////////////////////////////////////////////////////////////
struct Move {
    Move(int y, int x, int type, Piece piece);
    int x;          //New x value
    int y;          //New y value
    int type;       //Type of move, 0 without taking enemy piece, 1 with.
    Piece piece;    //Piece to be moved
};
///////////////////////////////////////
Move::Move(int y, int x, int type, Piece piece){
    this->x = x;
    this->y = y;
    this->type = type;
    this->piece = piece;
}

///////////////////////////////////////////////////////////////////////////////
class Board {
    public:
    Board(int size);
    void display();                          //Display game state
    void menu();                             //Display action menu
    
    bool move(int y, int x, int r);           //Move action sequence 2
    int doMove(Move move, int y, int x);     //Apply the selected move
    //Calculate possible moves
    int calculateMoves(int y, int x, Piece piece, int r);
    //Sub functions
    void calculateCheckerMoves(int values[4], int values2[4], Piece piece, int r);
    void calculateCheckerMoves2(int values[4], Piece piece, int r);
    void calculateKingMoves(int y, int x, Piece piece, int r);
    //Filter Moves, only allow take moves if possible
    void filterMoves();
    //Take an enemy piece and move over 2 squares
    bool takeMove(int values[4], Piece piece);
    //Move to an empty square
    bool normalMove(int values[4], Piece piece);

    int promote(Piece piece);               //Promote piece if possible
    bool checkBounds(int n);                //Check if value is within grid

    private:
    std::vector<Move> moves;                //Contains all the moves for the current player
    int scorePlayer1;                       
    int scorePlayer2;
    int currentPlayer;                      //Track which player's turn it is
    int size;                               //Size of the board
    std::unique_ptr<std::unique_ptr<Piece[]>[]> board; //Instance of the board
};
///////////////////////////////////////
Board::Board(int size){
    scorePlayer1 = 0;
    scorePlayer2 = 0;
    currentPlayer = 1;
    this->size = size;
    board.reset(new std::unique_ptr<Piece[]>[size]);

    for(int y = 0; y < size; y++){                    //Empty
        board[y] = std::unique_ptr<Piece[]>(new Piece[size]);
        for(int x = 0; x < size; x++){
            board[y][x] = Piece(0, 0, y, x);
        }
    }
    for(int y = 0; y < (size - 1) / 2; y++){          //Player 1
        for(int x = ((y+1)%2); x < size; x+=2){
            board[y][x] = Piece(1, 0, y, x);
        }
    }
    for(int y = size - 1; y > (size) / 2; y--){ //Player 2
        for(int x = ((y+1)%2); x < size; x+=2){
            board[y][x] = Piece(2, 0, y, x);
        }
    }
    for(int y = 0; y < size; y++){                    //White squares
        for(int x = (y%2); x < size; x+=2){
            board[y][x] = Piece(3, 0, y ,x);
        }
    }
}
///////////////////////////////////////
void Board::display(){
    //Top info
    std::cout << "Checkers " << size << "x" << size << std::endl;
    std::cout << "Current score:" << std::endl;
    std::cout << "P1:" << scorePlayer1 << " vs " << "P2:" << scorePlayer2 << std::endl; 

    //Top axis
    std::cout << "y\\x ";
    for(int x = 0; x < size; x++){
        std::cout << x << "   ";
    }

    std::cout << std::endl;
    for(int y = 0; y < size; y++){
        std::cout << y << " ";                        //Left axis
        for(int x = 0; x < size; x++){                //Grid
            std::cout << "[";
            if(board[y][x].player == 0){
                std::cout << "  ";
            }else if(board[y][x].player == 3){
                std::cout << "\u25A0\u25A0";
            }else{
                std::cout << board[y][x].player;
                std::cout << board[y][x].category;
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
///////////////////////////////////////
void Board::menu(){
    std::string str;
    int choice;

    while(true){
        display();
        std::cout << "Pick your choice player ";
        std::cout << currentPlayer << std::endl;
        std::cout << "1. Move a piece" << std::endl;
        std::cout << "-1. Quit" << std::endl;
        std::cin >> str; choice = atod<int>(str);
 
        if(choice == 1){
            if(move(0, 0, 0)){
                if(scorePlayer1 > scorePlayer2){
                    std::cout << "Player one "; 
                }else if(scorePlayer2 > scorePlayer1){
                    std::cout << "Player two";
                }else{
                    std::cout << "No one";
                }
                std::cout << "has won!" << std::endl;
                return;
            }
            currentPlayer = (currentPlayer == 1) ? 2 : 1; //Switch player
        }else if(choice == -1){
            return;
        }else{
            std::cout << "Invalid choice" << std::endl;
        }        
    }
}
///////////////////////////////////////
bool Board::move(int y, int x, int r){
    int choice;
    std::string str;
    moves.clear();
    
    if(r == 0){
        for(int y = 0; y < this->size; y++){
            for(int x = 0; x < this->size; x++){
                Piece piece = board[y][x];
                calculateMoves(y, x, piece, r);
            }
        }
    }else if (r == 1){
        Piece piece = board[y][x];
        calculateMoves(y, x, piece, r);
    }
    
    filterMoves();
    std::cout << "Available moves" << std::endl;
    if(moves.size() == 0){
        std::cout << "No valid moves available" << std::endl;
        if(r == 0){
            return true;
        }
        return false;
    }else{
        for(int i = 0; i < moves.size(); i++){
            Piece piece = moves[i].piece;
            std::cout << i + 1 << ". (" << piece.x << "," << piece.y << ") --->"; 
            std::cout << " (" << moves[i].x;std::cout << "," << moves[i].y << ")" << std::endl;
        }
        while(true){
            std::cout << "Enter an integer corresponding to the move" << std::endl;
            std::cin >> str;
            if(isdigit(str[0])){
                choice = atod<int>(str);
            }else{
                std::cout << "Invalid input" << std::endl;
                continue;
            }
            if(choice >= 1 && choice < moves.size() + 1){
                doMove(moves[choice-1], moves[choice-1].piece.y, moves[choice-1].piece.x);
                if(moves[choice-1].type == 1){//Player can move the same piece again after capture enemy piece
                    display();
                    return move(moves[choice-1].y, moves[choice-1].x, 1);
                }
                return false;
            }else{
                std::cout << "Invalid move" << std::endl;
                continue;
            }
        }
    }
    return false;
}
///////////////////////////////////////
void Board::filterMoves(){
    bool flag = false;
    for(int i = 0; i < moves.size(); i++){
        if(moves[i].type == 1){                 //Detect a take move
            flag = true;
        }
    }

    if(flag){                                   //Take move detected, remove all non take moves
        for(int i = moves.size() - 1; i >= 0; i--){
            if(moves[i].type != 1){
                moves.erase(moves.begin() + i);
            }
        }
    }
}
///////////////////////////////////////
int Board::doMove(Move move, int y, int x){
    int yjumps = move.y - y;
    int xjumps = move.x - x;
    
    //Place the empty pieces along the path of the move
    board[y][x] = Piece(0, 0, y, x);
    while(xjumps != 0 || yjumps != 0){
        yjumps > 0 ? yjumps--: yjumps++;
        xjumps > 0 ? xjumps--: xjumps++;
        board[y + yjumps][x + xjumps] = Piece(0, 0, y + yjumps, x + xjumps);
    }
    move.piece = Piece(move.piece.player, move.piece.category, move.y, move.x);
    board[move.y][move.x] = move.piece;     //Move the piece
    this->promote(move.piece);              //Promote the piece if possible
    if(move.type == 1){
        if(currentPlayer == 1){
            scorePlayer1++;
        }else if(currentPlayer == 2){
            scorePlayer2++;
        }
    }
    return 0;
}

///////////////////////////////////////
int Board::promote(Piece piece){
    if(((piece.y == 0) && (piece.player == 2))
        || ((piece.y == size - 1) && (piece.player == 1))){
        board[piece.y][piece.x].category = 1;
    }
    return 0;
}
///////////////////////////////////////
bool Board::checkBounds(int n){
    if((n >= 0) && (n < size)){
        return true;
    }
    return false;
}
///////////////////////////////////////
int Board::calculateMoves(int y, int x, Piece piece, int r){
    int left = x-1, right = x+1, up = y-1, down = y+1;
    int dleft = x-2, dright = x+2, dup = y-2, ddown = y+2; //double
    int arr0[4] = {left, down, ddown, dleft};
    int arr1[4] = {right, down, ddown, dright};
    int arr2[4] = {left, up, dup, dleft};
    int arr3[4] = {right, up, dup, dright};

    if(piece.category == 1 && piece.player == currentPlayer){   //Moves for king
        calculateKingMoves(y, x, piece, r);
    }else if(piece.category == 0){                              //Moves for normal checker
        if(piece.player == 1 && currentPlayer == 1){
            calculateCheckerMoves(arr0, arr1, piece, r);        //Can move backwards if able to capture
            calculateCheckerMoves(arr2, arr3, piece, 1);
        }else if(piece.player == 2 && currentPlayer == 2){
            calculateCheckerMoves(arr0, arr1, piece, 1);        //Can move backwards if able to capture
            calculateCheckerMoves(arr2, arr3, piece, r);
        }
    }
    return 0;
}
///////////////////////////////////////
void Board::calculateCheckerMoves(int values[4], int values2[4], Piece piece, int r){
    if(checkBounds(values[1])){
        calculateCheckerMoves2(values, piece, r);
        calculateCheckerMoves2(values2, piece, r);
    }
}
///////////////////////////////////////
void Board::calculateCheckerMoves2(int values[4], Piece piece, int r){
    if(checkBounds(values[0])){
        if(takeMove(values, piece)){  //Can always move to capture enemy piece
        }else if(r == 0){             //Can move to an empty space only on the first move after start of turn
            normalMove(values, piece);
        }
    }
}
///////////////////////////////////////
void Board::calculateKingMoves(int y, int x, Piece piece, int r){
    int arr_x[4] = {-1, -1, 1, 1};
    int arr_y[4] = {-1, 1, -1, 1};
    int opponentPlayer = (currentPlayer == 1) ? 2 : 1;

    for(int n = 0; n < 4; n++){
        int i_y = y;
        int i_x = x;
        for(int i = 0; i < size; i++){
            i_y += arr_y[n];
            i_x += arr_x[n];
            if(checkBounds(i_y) && checkBounds(i_x)){
                if(board[i_y][i_x].player == 0 && r == 0){
                    moves.push_back(Move(i_y, i_x, 0, piece));
                }else if(board[i_y][i_x].player == opponentPlayer){
                    if(checkBounds(i_y + arr_y[n]) && checkBounds(i_x + arr_x[n])){
                        if(board[i_y + arr_y[n]][i_x + arr_x[n]].player == 0){
                            moves.push_back(Move(i_y + arr_y[n], i_x + arr_x[n], 1, piece));
                        }
                    }
                    break;
                }
            }else{
                break;
            }
        }
    }
}
///////////////////////////////////////
bool Board::takeMove(int values[4], Piece piece){
    int opponentPlayer = (currentPlayer == 1) ? 2 : 1;

    if(board[values[1]][values[0]].player == opponentPlayer){
        if  (checkBounds(values[2]) && checkBounds(values[3])
            && board[values[2]][values[3]].player == 0){
            moves.push_back(Move(values[2], values[3], 1, piece));
            return true;
        }
    }
    return false;
}
///////////////////////////////////////
bool Board::normalMove(int values[4], Piece piece){
    if(board[values[1]][values[0]].player == 0){
        moves.push_back(Move(values[1], values[0], 0, piece));
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
int main () {
    std::string str;
    int size = 0;

    std::cout << "Enter the size of the board." << std::endl;
    std::cout <<  "It must be at least 6 ";
    std::cout << "at most 10, and a multiple of 2" << std::endl;

    //Limited the size to 10 otherwise display doesnt work properly
    while(size < 6 || size > 10 || size % 2 == 1){
        std::cin >> str;
        size = atod<int>(str);
        if(size < 6 || size > 10 || size % 2 == 1){
            std::cout << "Please try again" << std::endl;
        }
    }
    std::unique_ptr<Board> board(new Board(size));
    board->display();
    board->menu();
    return 0;
}