#include "board.h"

using namespace board_utils;

board::move::move() {
    start_pos = end_pos = piece_type = capture_position = capture_piece = old_ply_100 = -1;
    castle_disruptions.reset();
}

board::move board::move_from_pair(pair<int, int> arg) const {
    board::move res;
    res.start_pos = arg.first;
    res.end_pos = arg.second; //i hate c++ why doesnt structured binding work for such cases

    res.old_ply_100 = ply_100;

    if(res.start_pos<0) { //promotion
        res.start_pos *= -1;
        res.promotion_type = res.end_pos % 4;
        res.end_pos >>= 2;
    }

    for(res.piece_type = 6*turn; res.piece_type < (6 * turn) + 6; res.piece_type++)
        if(is_piece[res.piece_type][res.start_pos]) {
            break;
        }

    if(res.piece_type == turn * 6 && (res.start_pos - res.end_pos + 64)%8 != 0 && !is_anything[res.end_pos]) {
        res.capture_position = ind_from_coordinate({res.start_pos/8, res.end_pos%8});
        for(res.capture_piece = 6-6*turn; res.capture_piece < 12-6*turn; res.capture_piece++)
            if(is_piece[res.capture_piece][res.capture_position]) break;

    } 
    else {
        if(is_color[!turn][res.end_pos]) {
            res.capture_position = res.end_pos;
            for(res.capture_piece = 6-6*turn; res.capture_piece < 12-6*turn; res.capture_piece++)
                if(is_piece[res.capture_piece][res.capture_position]) break;
        }
    }

    if(castle[0] && (res.start_pos == 4 || res.start_pos == 7 || res.end_pos == 7))  res.castle_disruptions[0] = true;
    if(castle[1] && (res.start_pos == 4 || res.start_pos == 0 || res.end_pos == 0))  res.castle_disruptions[1] = true;
    if(castle[2] && (res.start_pos == 60|| res.start_pos == 63|| res.end_pos == 63)) res.castle_disruptions[2] = true;
    if(castle[3] && (res.start_pos == 60|| res.start_pos == 56|| res.end_pos == 56)) res.castle_disruptions[3] = true;

    return res;
}

