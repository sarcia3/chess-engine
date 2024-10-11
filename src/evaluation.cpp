#include "board.h"
#include "bitboard.h"
#include <iostream>
constexpr std::array<float , 12> piece_val = {1, 3, 3, 4.5, 9, 0, 1, 3, 3, 4.5, 9, 0};

float board::evaluate() const{
    if(current_state == white_won) return 999999 * -1 * (2*turn - 1);
    if(current_state == draw_3_fold ||
       current_state == draw_50_rule ||
       current_state == draw_stalemate) return 0;
    if(current_state == black_won) return 999999 * (2*turn - 1);

    float res=0;
    for(int i=0; i<12; i++) {
        if(6*turn <= i && i < 6*turn + 6) res += popcount(is_piece[i]) * piece_val[i];
        else res -= popcount(is_piece[i]) * piece_val[i];
    }

    return res;
}

float board::evaluate_white_pov() const{
    return (turn ? -evaluate() : evaluate());
}

/*board copy(*this);
346         copy.make_move(top, false);
*/

pair<float, pair<int, int>> board::negaMax_search(int depth) const {
    if(depth <= 0) return {evaluate(), {-1, -1}};
    
//    cerr <<depth<<'\n';

    pair<float, pair<int, int>> max = {-9999999, {-1, -1}};
    auto moves = gen_moves();
    while(moves.size()){
//        cerr<< "depth: " << depth << " moves left: " << moves.size() << '\n';
        auto move = moves.top(); moves.pop();
        board copy(*this);
        copy.make_move(move, true);
        auto score = copy.negaMax_search(depth-1);
        score.first *= -1;
        if(score.first > max.first)
            max = score;
    }
    return max;
}
