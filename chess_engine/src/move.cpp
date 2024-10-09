#include "board.h"

board::move::move() {
    start_pos = end_pos = piece_type = capture_position = capture_piece = -1;
    wkc_disrupted= wqc_disrupted = bkc_disrupted = bqc_disrupted = false;
}

board::move::move(pair<int, int> arg) {
    int start_pos = arg.first;
    int end_pos = arg.second;
    for(piece_type = turn*6; piece_type < turn * 6 + 6; piece_type++)
        if(is_piece[piece_type][start_pos]) break;

    if(piece_type == 6 * turn && (start_pos-end_pos + 64)%8 != 0) {
        capture_position = ind_from_coordinate({start_pos/8, end_pos%8});
        capture_piece = 6 - 6*turn;
    } // en pessant
    else {
        if(is_color[!turn][end_pos]) {
            capture_position = end_pos;
            for(capture_piece = 6-6*turn; capture_piece < 12-6*turn; capture_piece++)
                if(is_piece[capture_piece][capture_position]) break;
        }
    }
    if(white_short_castle && (start_pos & 11ULL) || end_pos & 90ULL) wkc_disrupted = true;
    if(white_long_castle && (start_pos & 11ULL) || end_pos & 11ULL) wkc_disrupted = true;
    if(white_short_castle && (start_pos & 11ULL) || end_pos & 11ULL) wkc_disrupted = true;
    if(white_short_castle && (start_pos & 11ULL) || end_pos & 11ULL) wkc_disrupted = true;
}
