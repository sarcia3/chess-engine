#include <iostream> 
#include "board.h"  

void clearConsole() {
#ifdef _WIN32 
    system("cls");
#else 
    system("clear");
#endif
}

int main() {
    clearConsole();
    board start("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    /*cout <<"Evaluation: " << start.negaMax_search(4).first << '\n';*/

    auto legal = start.print_moves();
    start.print_board();
    while(legal.size()) {
//        cout <<"Evaluation: " << start.negaMax_search(3).first << '\n';
 //       cout <<"move: " << start.negaMax_search(3).second.first << start.negaMax_search(3).second.second << '\n';
        start.user_move(legal);
        clearConsole();
        legal = start.print_moves();
        start.print_board();
    }
    return 0; 
}


