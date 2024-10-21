#include "board.h"
#include "bitboard.h"
#include <iostream>
constexpr std::array<float , 12> piece_val = {1, 3, 3, 4.5, 9, 0, 1, 3, 3, 4.5, 9, 0};

float board::evaluate() const{
    if(current_state == white_won) return 999999 * -1 * (2*turn - 1);
    if(current_state == black_won) return 999999 * (2*turn - 1);

    int res=0;
    for(int i=0; i<12; i++) {
        if(6*turn <= i && i < 6*turn + 5) res += popcount(is_piece[i]) * piece_val[i];
        else res -= popcount(is_piece[i]) * piece_val[i];
    }

    return res;
}

float board::evaluate_white_pov() {
    return (turn ? -evaluate() : evaluate());
}

float board::negaMax_search(int depth) {
    if(depth <= 0) return evaluate();

    float max = -9999999;
    auto moves = gen_moves();
    if(current_state == draw_stalemate && moves.empty()) return 0;

    while(moves.size()){
        auto move = moves.top(); moves.pop();
        make_move(move, false);
        auto score = -negaMax_search(depth-1);
        undo_move(move);
        if(score > max)
            max = score;
    }
    return max;
}

board::move board::get_best_move(int depth) {
    float max = -9999999;
    move res;

    auto moves = gen_moves();
    while(moves.size()){
        auto move = moves.top(); moves.pop();
        make_move(move, true);
        auto score = -negaMax_search(depth-1);
        undo_move(move);
        if(score > max) {
            max = score;
            res = move;
        }
    }
    return res;
}

string board::print_best_move(int depth) {
    auto tmp = get_best_move(depth);
    return board_utils::move_readable({tmp.start_pos, tmp.end_pos});
}
