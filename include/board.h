#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"
#include "board_utils.h"
#include <stack>
#include <map>
#include <vector>
#include <functional>
#include <array>
#include <bit>
#include <iostream>
#include <cmath> 
#include <set>
#include <bitset>

using namespace std;

class board {
    private:
        struct move{
            int start_pos = -1;
            int end_pos = -1;
            int piece_type = -1;
            int capture_position = -1;
            int capture_piece = -1;
            int promotion_type = -1;
            int old_ply_100 = -1;
            bitset<4> castle_disruptions;
            pair<int, int> old_en_pessant = {-1, -1};
            void display() const;
            move();
        };

        move move_from_pair(pair<int, int> arg) const;

        array<bitboard, 12> is_piece;
        bitset<4> castle;
        int ply_100;
        int ply;
        bool turn;
        pair<int, int> en_pessant;
        
        bitboard &white_pawn;
        bitboard &white_knight;
        bitboard &white_bishop;
        bitboard &white_rook;
        bitboard &white_queen;
        bitboard &white_king;
        bitboard &black_pawn;
        bitboard &black_knight;
        bitboard &black_bishop;
        bitboard &black_rook;
        bitboard &black_queen;
        bitboard &black_king;

        bitboard is_anything;
        array<bitboard, 2> is_color;

        bitboard &is_white;
        bitboard &is_black;

        board();
        board(const board& to_copy);

        void update_is_anything_color();

        bitboard gen_attacked(int gen_turn) const;
        stack<move> gen_moves();

        bool is_legal() const;

        void make_move(const move &arg, bool real);
        void make_move(const pair<int, int> &arg, bool real);
        void make_move(const pair<int, int> &start, const pair<int, int> &end, bool real);
        //the need for bool real arises from the fact that only after actual moves
        //we want to update the game state
        //if we had always tried to update we would have an infinite loop
        void undo_move(const move &arg);


    public: 
        enum game_state {undecided, white_won, draw_3_fold, draw_50_rule, draw_stalemate, black_won};
        
        game_state current_state;

        void update_state();

        set<string> print_moves();

        void user_move(set<string> legal);

        board (const string &fen);

        void print_board() const;

        bool get_turn() const;

        float evaluate() const;

        float evaluate_white_pov();
        
        pair<float, pair<int, int>> negaMax_search(int depth);

        int perft(int depth, int org_depth);

};
#endif
