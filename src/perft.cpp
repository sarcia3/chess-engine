#include "board.h"

int board::perft(int depth, int org_depth) {
    if(depth == 0) {
        return 1;
    }
    auto moves = gen_moves();
    int res = 0;
    while(moves.size()){
        auto move = move_from_pair(moves.top());
        moves.pop();
        make_move(move, false);
        auto perft_res = perft(depth-1, org_depth);
        if(depth == org_depth)
            cout << perft_res << ' ' << move.start_pos << ' ' << move.end_pos << '\n';
        res += perft_res;
        undo_move(move);
    }
    return res;
}
