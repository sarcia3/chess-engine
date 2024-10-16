#include "board.h"

int board::perft(int depth, int org_depth) {
    if(depth == 1) {
        return gen_moves().size();
    }
    auto moves = gen_moves();
    int res = 0;
    while(moves.size()){
        auto move = moves.top();
        moves.pop();
        make_move(move, false);
        auto perft_res = perft(depth-1, org_depth);
        if(depth == org_depth)
            cout << perft_res << ' ' << board_utils::move_readable({move.start_pos, move.end_pos}) << '\n';
        res += perft_res;
        undo_move(move);
    }
    return res;
}
