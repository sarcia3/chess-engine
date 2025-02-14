#include "board.h"
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

using namespace std;
using namespace board_utils;

board::board()
    : is_piece{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      white_pawn(is_piece[0]),
      white_knight(is_piece[1]),
      white_bishop(is_piece[2]),
      white_rook(is_piece[3]),
      white_queen(is_piece[4]),
      white_king(is_piece[5]),
      black_pawn(is_piece[6]),
      black_knight(is_piece[7]),
      black_bishop(is_piece[8]),
      black_rook(is_piece[9]),
      black_queen(is_piece[10]),
      black_king(is_piece[11]),
      is_anything(0),
      is_color{0, 0},
      is_white(is_color[0]),
      is_black(is_color[1]),
      current_state(undecided)
{
    castle.reset();
    ply_100 = 0;
    ply = 0;
    turn = 0;
    en_pessant = {-1, -1};
}

board::board(const board& to_copy)
    : is_piece(to_copy.is_piece),
      white_pawn(is_piece[0]),
      white_knight(is_piece[1]),
      white_bishop(is_piece[2]),
      white_rook(is_piece[3]),
      white_queen(is_piece[4]),
      white_king(is_piece[5]),
      black_pawn(is_piece[6]),
      black_knight(is_piece[7]),
      black_bishop(is_piece[8]),
      black_rook(is_piece[9]),
      black_queen(is_piece[10]),
      black_king(is_piece[11]),
      is_anything(to_copy.is_anything),
      is_color(to_copy.is_color),
      is_white(is_color[0]),
      is_black(is_color[1]),
      current_state(to_copy.current_state)
{
    castle = to_copy.castle;
    ply_100 = to_copy.ply_100;
    ply = to_copy.ply;
    turn = to_copy.turn;
    en_pessant = to_copy.en_pessant;
}

void board::update_is_anything_color() {
    is_white = 0;

    for(int i=0; i<6; i++)
        is_white |= is_piece[i];

    is_black = 0;

    for(int i=6; i<12; i++)
        is_black |= is_piece[i];

    is_anything = is_white | is_black;
}

bitboard board::gen_attacked(int gen_turn) const {
    bitboard res = 0;

    auto set_if_legal = [&](const pair<int, int> &arg) {
        if(coordinate_is_legal(arg)) res.set_val(true, ind_from_coordinate(arg));
    };

    if(gen_turn == 0) {
        bitboard copy = white_pawn;
        for(int i = copy.get_first_one(); i<64; i=copy.get_first_one()) {
            copy.set_val(false, i);
            auto [row, column] = gen_coordinate(i);

            set_if_legal({row+1, column-1});
            set_if_legal({row+1, column+1});
        }
    } else {
        bitboard copy = black_pawn;
        for(int i = copy.get_first_one(); i<64; i=copy.get_first_one()) {
            copy.set_val(false, i);
            auto [row, column] = gen_coordinate(i);

            set_if_legal({row-1, column-1});
            set_if_legal({row-1, column+1});
        }
    }

    const bitboard &turn_knight = is_piece[1 + 6 * gen_turn];
    const bitboard &turn_bishop = is_piece[2 + 6 * gen_turn];
    const bitboard &turn_rook   = is_piece[3 + 6 * gen_turn];
    const bitboard &turn_queen  = is_piece[4 + 6 * gen_turn];
    const bitboard &turn_king   = is_piece[5 + 6 * gen_turn];

    bitboard copy = turn_knight;
    for(int i = copy.get_first_one(); i<64; i=copy.get_first_one()) {
        copy.set_val(false, i);
        auto [row, column] = gen_coordinate(i);

        for(int dir1=-1; dir1<2; dir1+=2)
            for(int dir2=-1; dir2<2; dir2+=2) {
                set_if_legal({row + 2 * dir1, column + 1 * dir2});
                set_if_legal({row + 1 * dir1, column + 2 * dir2});
            }
    }

    pair<int, int> direction;
    function<void(pair<int, int>)> go_into = [&](pair<int, int> coordinate) {
        coordinate.first += direction.first;
        coordinate.second += direction.second;
        if(coordinate_is_legal(coordinate)){
            set_if_legal(coordinate);
            if(!((is_anything >> ind_from_coordinate(coordinate))&1))
                go_into(coordinate);
        }
    };

    copy = turn_bishop;
    for(int i = copy.get_first_one(); i<64; i=copy.get_first_one()){
        copy.set_val(false, i);
        for(direction.first = -1; direction.first<2; direction.first+=2)
            for(direction.second = -1; direction.second<2; direction.second+=2)
                go_into(gen_coordinate(i));
    }

    copy = turn_rook;
    for(int i = copy.get_first_one(); i<64; i=copy.get_first_one()){
        copy.set_val(false, i);
        for(direction.first = -1; direction.first<2; direction.first++)
            for(direction.second = -1; direction.second<2; direction.second++)
                if(((bool)direction.first) ^ ((bool)direction.second))
                    go_into(gen_coordinate(i));
    }

    copy = turn_queen;
    for(int i = copy.get_first_one(); i<64; i=copy.get_first_one()){
        copy.set_val(false, i);
        for(direction.first = -1; direction.first<2; direction.first++)
           for(direction.second = -1; direction.second<2; direction.second++)
               if(direction.first != 0 || direction.second != 0)
                   go_into(gen_coordinate(i));
    }

    return res;
}

bool board::is_legal() const{
    //1st check - is every square occupied by exactly zero or one piece
    
    bitboard current = 0;
    for(const auto &elem : is_piece)
        if(current & elem)
            return false;
        else current |= elem;

    //2nd check - pawns on first and last ranks
    if(0xFF000000000000FF & (white_pawn | black_pawn))
        return false;


    //3rd check - exactly one king of each color exists
    if(popcount(white_king) != 1 || popcount(black_king) != 1)
        return false;

    //4th check - king not in check
    if(gen_attacked(turn) & is_piece[11 - 6*turn])
        return false;

    return true;
};

bool board::is_legal_after_move() const{
    return !(gen_attacked(turn) & is_piece[11 - 6*turn]);
};
stack<board::move> board::gen_moves() {
    auto old = is_piece;
    stack<pair<int, int>> S;

    auto pawn_push = [&](int start, int end){
        if(end/8 == 0 || end/8 == 7){
            end<<=2;
            S.push({-start, end});
            S.push({-start, end+1});
            S.push({-start, end+2});
            S.push({-start, end+3});
            //negative start to signify pawn promotion, last two bits of end representing
            //the new piece 0 - knight, 1 - bishop, 2 - rook, 3 - queen
        } else {
            S.push({start, end});
        }
    };

    if(turn == 0) {
        bitboard copy = white_pawn;
        for(int i=copy.get_first_one(); i<64; i=copy.get_first_one()) {
            copy.set_val(false, i);
            auto [row, column] = gen_coordinate(i);
            if(coordinate_is_legal({row+1, column-1}) &&
                ((is_black[ind_from_coordinate({row+1, column-1})]) || (en_pessant.first == row+1 && en_pessant.second == column-1)))
                   pawn_push(i, ind_from_coordinate({row+1, column-1}));
            if(coordinate_is_legal({row+1, column+1}) &&
                (is_black[ind_from_coordinate({row+1, column+1})] || (en_pessant.first == row+1 && en_pessant.second == column+1)))
                    pawn_push(i, ind_from_coordinate({row+1, column+1}));

            if(!is_anything[i+8]) {
                pawn_push(i, i+8);
                if(row==1 && !is_anything[i+16])
                    S.push({i, i+16}); // pawn push useless, thus omited
            }
        }
    } else {
        bitboard copy = black_pawn;
        for(int i=copy.get_first_one(); i<64; i = copy.get_first_one()) {
            copy.set_val(false, i);
            auto [row, column] = gen_coordinate(i);
            if(coordinate_is_legal({row-1, column-1}) &&
                (is_white[ind_from_coordinate({row-1, column-1})] || (en_pessant.first == row-1 && en_pessant.second == column-1)))
                    pawn_push(i, ind_from_coordinate({row-1, column-1}));
            if(coordinate_is_legal({row-1, column+1}) &&
                (is_white[ind_from_coordinate({row-1, column+1})] || (en_pessant.first == row-1 && en_pessant.second == column+1)))
                    pawn_push(i, ind_from_coordinate({row-1, column+1}));

            if(!is_anything[i-8]) {
                pawn_push(i, ind_from_coordinate({row-1, column}));
                if(row==6 && !is_anything[i-16])
                    S.push({i, i-16}); // pawn push useless, thus omited
            }
        }
    }

    const bitboard &turn_knight = is_piece[1 + 6 * turn];
    const bitboard &turn_bishop = is_piece[2 + 6 * turn];
    const bitboard &turn_rook   = is_piece[3 + 6 * turn];
    const bitboard &turn_queen  = is_piece[4 + 6 * turn];
    const bitboard &turn_king   = is_piece[5 + 6 * turn];

    bitboard copy = turn_knight;
    for(int i=copy.get_first_one(); i<64; i = copy.get_first_one()) {
        copy.set_val(false, i);
        auto [row, column] = gen_coordinate(i);

        for(int ska1=-1; ska1<2; ska1+=2)
            for(int ska2=-1; ska2<2; ska2+=2) {
                if(coordinate_is_legal({row + 2 * ska1, column + 1 * ska2}) &&
                    !is_color[turn][ind_from_coordinate({row + 2 * ska1, column + 1 * ska2})])
                        S.push({i, ind_from_coordinate({row + 2 * ska1, column + 1 * ska2})});
                if(coordinate_is_legal({row + 1 * ska1, column + 2 * ska2}) &&
                    !is_color[turn][ind_from_coordinate({row + 1 * ska1, column + 2 * ska2})])
                        S.push({i, ind_from_coordinate({row + 1 * ska1, column + 2 * ska2})});
            }
    }
    copy = turn_king;
    for(int i=copy.get_first_one(); i<64; i=copy.get_first_one()){
        copy.set_val(false, i);
        auto [row, column] = gen_coordinate(i);
        for(int dirx=-1; dirx<2; dirx++)
            for(int diry=-1; diry<2; diry++)
                if(coordinate_is_legal({row + diry, column + dirx}) &&
                    !is_color[turn][ind_from_coordinate({row + diry, column + dirx})])
                        S.push({i, ind_from_coordinate({row + diry, column + dirx})});

        break;
    }
    int i;
    pair<int, int> direction;
    function<void(pair<int, int>)> go_into = [&](pair<int, int> coordinate) {
        coordinate.first += direction.first;
        coordinate.second += direction.second;
        if(coordinate_is_legal(coordinate) && !is_color[turn][ind_from_coordinate(coordinate)]){
            S.push({i, ind_from_coordinate(coordinate)});
            if(!is_anything[ind_from_coordinate(coordinate)])
                go_into(coordinate);
        }
    };
    copy = turn_bishop;
    for(i = copy.get_first_one(); i<64; i=copy.get_first_one()){
        copy.set_val(false, i);
        for(direction.first = -1; direction.first<2; direction.first+=2)
            for(direction.second = -1; direction.second<2; direction.second+=2)
                go_into(gen_coordinate(i));
    }
    copy = turn_rook;
    for(i = copy.get_first_one(); i<64; i=copy.get_first_one()){
        copy.set_val(false, i);
        for(direction.first = -1; direction.first<2; direction.first++)
            for(direction.second = -1; direction.second<2; direction.second++)
                if(((bool)direction.first) ^ ((bool)direction.second))
                    go_into(gen_coordinate(i));
    }

    copy = turn_queen;
    for(i = copy.get_first_one(); i<64; i=copy.get_first_one()){
        copy.set_val(false, i);
        for(direction.first = -1; direction.first<2; direction.first++)
           for(direction.second = -1; direction.second<2; direction.second++)
               if(direction.first != 0 || direction.second != 0)
                   go_into(gen_coordinate(i));
    }

    stack<move> res;

    if(turn == 0 && !gen_attacked(!turn)[4]) {
        if(castle[0] && !((is_anything | gen_attacked(!turn)) & (96ULL))){
            res.push(move_from_pair({0, 0}));
        }
        if(castle[1] && !(is_anything & 14ULL) && !(gen_attacked(!turn) & 12ULL)){
            res.push(move_from_pair({1, 1}));
        }
    } else if(!gen_attacked(!turn)[60]){
        if(castle[2] && !((is_anything | gen_attacked(!turn)) & (0x6000000000000000ULL))){
            res.push(move_from_pair({2, 2}));
        }
        if(castle[3] && (!(is_anything & 0xE00000000000000ULL)) && (!(gen_attacked(!turn) & 0xC00000000000000ULL))){
            res.push(move_from_pair({3, 3}));
        }
    }

    while(S.size()) {
        auto top = S.top(); S.pop();
        auto move = move_from_pair(top);
        make_move(move, false);
        if(is_legal_after_move()) res.push(move);
        undo_move(move);
    }

    if(ply_100 == 100) return {};
    if(turn == 0 && res.empty()) if(white_king & gen_attacked(!turn)) {current_state = black_won; return {};}
    if(turn == 1 && res.empty()) if(black_king & gen_attacked(!turn)) {current_state = white_won; return {};}
    if(res.empty()) {current_state = draw_stalemate; return {};}

    return res;
}


void board::update_iac(bool val, bool color, int i){
    is_anything.set_val(val, i);
    is_color[color].set_val(val, i);
}

void board::make_move(const move &arg, bool real){
    auto set_val_update_iac = [&] (bitboard &bb, bool val, bool color, int ind) {
        bb.set_val(val, ind);
        is_anything.set_val(val, ind);
        is_color[color].set_val(val, ind);
    };

    if(arg.promotion_type != -1)
        { set_val_update_iac(is_piece[arg.promotion_type], true, turn, arg.end_pos); ply_100=0; }

    if(arg.capture_piece != -1)
        { set_val_update_iac(is_piece[arg.capture_piece], false, !turn, arg.capture_position); ply_100=0; }


    if(arg.start_pos == arg.end_pos) {
        ply_100 = 0;
        if(arg.start_pos == 0) {
            set_val_update_iac(white_king, false, turn, 4);
            set_val_update_iac(white_king, true, turn, 6);
            set_val_update_iac(white_rook, false, turn, 7);
            set_val_update_iac(white_rook, true, turn, 5);
        }
        if(arg.start_pos == 1) {
            set_val_update_iac(white_king, false, turn, 4);
            set_val_update_iac(white_king, true, turn, 2);
            set_val_update_iac(white_rook, false, turn, 0);
            set_val_update_iac(white_rook, true, turn, 3);
        }

        if(arg.start_pos == 2) {
            set_val_update_iac(black_king, false, turn, 56+4);
            set_val_update_iac(black_king, true, turn, 56+6);
            set_val_update_iac(black_rook, false, turn, 56+7);
            set_val_update_iac(black_rook, true, turn, 56+5);
        }
        if(arg.start_pos == 3) {
            set_val_update_iac(black_king, false, turn, 56+4);
            set_val_update_iac(black_king, true, turn, 56+2);
            set_val_update_iac(black_rook, false, turn, 56+0);
            set_val_update_iac(black_rook, true, turn, 56+3);
        }
        ply_100=0;
    } else {
        set_val_update_iac(is_piece[arg.piece_type], false, turn, arg.start_pos);
        if(arg.promotion_type == -1) set_val_update_iac(is_piece[arg.piece_type], true, turn, arg.end_pos);
    }

    if(arg.piece_type == 6*turn && abs(arg.start_pos - arg.end_pos) == 16) 
        en_pessant = gen_coordinate((arg.start_pos + arg.end_pos)/2); //we won't get an error caused by rounding-up
    else en_pessant = {-1, -1};
    ply_100++;
    ply++;
    turn^=1;
    if(real) update_state();
}

void board::make_move(const pair<int, int> &arg, bool real) {
    make_move(move_from_pair(arg), real);
}

void board::make_move(const pair<int, int> &start, const pair<int, int> &end, bool real){
    make_move(move_from_pair({ind_from_coordinate(start), ind_from_coordinate(end)}), real);
}

void board::undo_move(const board::move &arg) {
    turn ^= 1;
    castle |= arg.castle_disruptions;
    en_pessant = arg.old_en_pessant;

    if(arg.start_pos == arg.end_pos) {
        if(arg.start_pos == 0) {
            white_king.set_val(true, 4);  update_iac(true, turn, 4);
            white_king.set_val(false, 6); update_iac(false, turn, 6);
            white_rook.set_val(true, 7);  update_iac(true, turn, 7);
            white_rook.set_val(false, 5); update_iac(false, turn, 5);
        }
        if(arg.start_pos == 1) {
            white_king.set_val(true, 4);  update_iac(true, turn, 4);
            white_king.set_val(false, 2); update_iac(false, turn, 2);
            white_rook.set_val(true, 0);  update_iac(true, turn, 0);
            white_rook.set_val(false, 3); update_iac(false, turn, 3);
        }
        if(arg.start_pos == 2) {
            black_king.set_val(true, 56+4);  update_iac(true, turn, 56+4);
            black_king.set_val(false, 56+6); update_iac(false, turn, 56+6);
            black_rook.set_val(true, 56+7);  update_iac(true, turn, 56+7);
            black_rook.set_val(false, 56+5); update_iac(false, turn, 56+5);
        }
        if(arg.start_pos == 3) {
            black_king.set_val(true, 56+4);  update_iac(true, turn, 56+4);
            black_king.set_val(false, 56+2); update_iac(false, turn, 56+2);
            black_rook.set_val(true, 56+0);  update_iac(true, turn, 56+0);
            black_rook.set_val(false, 56+3); update_iac(false, turn, 56+3);
        }
    } else {
        is_piece[arg.piece_type].set_val(false, arg.end_pos);  update_iac(false, turn, arg.end_pos);
        is_piece[arg.piece_type].set_val(true, arg.start_pos); update_iac(true, turn, arg.start_pos);
    }

    if(arg.promotion_type != -1)
        { is_piece[arg.promotion_type].set_val(false, arg.end_pos); update_iac(false, turn, arg.end_pos); }

    if(arg.capture_piece != -1)
        { is_piece[arg.capture_piece].set_val(true, arg.capture_position); update_iac(true, !turn, arg.capture_position); }

    ply_100 = arg.old_ply_100;
    ply--;
};

void board::update_state(){
    if(ply_100 == 100) {current_state = draw_50_rule; return;}
    if(turn == 0) {if(black_king & gen_attacked(turn)) {current_state = white_won; return;}}
    if(turn == 1) {if(white_king & gen_attacked(turn)) {current_state = black_won; return;}}
    if(gen_moves().size() == 0) {current_state == draw_stalemate; return;}
}

set<string> board::print_moves() {
    cout << "Avalaible moves";
    auto tmp = gen_moves();
    set<string> res;
    if(tmp.size() == 0) {
        cout << ": none!\n";
        switch (current_state) {
            case white_won : cout << "White won!\n"; break;
            case draw_3_fold : cout << "Draw! (a 3-fold repetition)\n"; break;
            case draw_50_rule : cout << "Draw! (50-move rule)\n"; break;
            case draw_stalemate : cout << "Draw by stalemate!\n"; break;
            case black_won : cout << "Black won!\n"; break;
        }
        return {};
    }

    cout << " (" << tmp.size() << ")\n";
    while(tmp.size()) {
        auto top = tmp.top(); tmp.pop();
        if(top.start_pos == top.end_pos){
            cout << (top.start_pos % 2 ? "o-o-o\n" : "o-o\n");
            (top.start_pos % 2 ? res.insert("o-o-o") : res.insert("o-o"));
            continue;
        }
        if(top.promotion_type != -1) {
            string without_prom = (std::string(1, char(gen_coordinate(top.start_pos).second + 'a')) +
                      std::to_string(gen_coordinate(top.start_pos).first + 1) + '-' +
                      std::string(1, char(gen_coordinate(top.end_pos).second + 'a')) +
                      std::to_string(gen_coordinate(top.end_pos).first + 1) + '=');

            if(top.promotion_type == 1){
                without_prom+=("N");
                cout << without_prom << '\n';
                res.insert(without_prom);
                continue;
            }
            if(top.promotion_type == 2){
                without_prom+=("B");
                cout << without_prom << '\n';
                res.insert(without_prom);
                continue;
            }
            if(top.promotion_type == 3){
                without_prom+=("R");
                cout << without_prom << '\n';
                res.insert(without_prom);
                continue;
            }
            if(top.promotion_type == 4){
                without_prom+=("Q");
                cout << without_prom << '\n';
                res.insert(without_prom);
                continue;
            }
        }
        string move = (std::string(1, char(gen_coordinate(top.start_pos).second + 'a')) +
                      std::to_string(gen_coordinate(top.start_pos).first + 1) + '-' +
                      std::string(1, char(gen_coordinate(top.end_pos).second + 'a')) +
                      std::to_string(gen_coordinate(top.end_pos).first + 1));

        cout << move << '\n';
        res.insert(move);
    }
    return res;
}

void board::user_move(set<string> legal){
    cout << "input move: ";
    string s; cin >> s;
    while(legal.count(s) == 0) {
        cout << "Illegal move! Try again.\ninput move: ";
        cin >> s;
    }

    if(s == "o-o") {make_move({0 + 2*turn, 0 + 2*turn}, true); return;}
    if(s == "o-o-o") {make_move({1 + 2*turn, 1 + 2*turn}, true); return;}
    if(s.size()==7) {
        int piece;
        if(s.back() == 'N') piece = 0;
        if(s.back() == 'B') piece = 1;
        if(s.back() == 'R') piece = 2;
        if(s.back() == 'Q') piece = 3;
        make_move({-ind_from_coordinate({s[1]-'1', s[0]-'a'}), (ind_from_coordinate({s[4]-'1', s[3]-'a'})<<2) + piece}, true);
        return;
    }
    make_move({s[1]-'1', s[0]-'a'}, {s[4]-'1', s[3]-'a'}, true);
}

board::board (const string &fen)
    : is_piece{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      white_pawn(is_piece[0]),
      white_knight(is_piece[1]),
      white_bishop(is_piece[2]),
      white_rook(is_piece[3]),
      white_queen(is_piece[4]),
      white_king(is_piece[5]),
      black_pawn(is_piece[6]),
      black_knight(is_piece[7]),
      black_bishop(is_piece[8]),
      black_rook(is_piece[9]),
      black_queen(is_piece[10]),
      black_king(is_piece[11]),
      is_anything(0),
      is_color{0, 0},
      is_white(is_color[0]),
      is_black(is_color[1]),
      current_state(undecided)
{
    constexpr array<int, 128> parse = []() {
        array<int, 128> map{};

        map['P'] = 0;
        map['N'] = 1;
        map['B'] = 2;
        map['R'] = 3;
        map['Q'] = 4;
        map['K'] = 5;
        map['p'] = 6;
        map['n'] = 7;
        map['b'] = 8;
        map['r'] = 9;
        map['q'] = 10;
        map['k'] = 11;

        return map;
    }();

    int fen_pos=0;
    for(int j = 7; j >= 0; j--) {
        int i = 0;
        for(; fen[fen_pos] != '/' && fen[fen_pos] != ' '; fen_pos++) {
            if(fen[fen_pos] >= '0' && fen[fen_pos] <= '9')
                i += fen[fen_pos] - '0';
            else
                is_piece[parse[fen[fen_pos]]].set_val(true, ind_from_coordinate({j, i++}));
        }
        fen_pos++;
    }

    turn = (fen[fen_pos] != 'w');

    fen_pos++;
    fen_pos++;
    if(fen[fen_pos] != '-'){
        while(fen[fen_pos] != ' '){
            castle[0] = castle[0] | (fen[fen_pos] == 'K');
            castle[1] = castle[1] | (fen[fen_pos] == 'Q');
            castle[2] = castle[2] | (fen[fen_pos] == 'k');
            castle[3] = castle[3] | (fen[fen_pos] == 'q');

            fen_pos++;
        }
    } else fen_pos++;

    fen_pos++;
    if(fen[fen_pos] != '-')
        en_pessant = {fen[fen_pos+1]-'1', fen[fen_pos++]-'a'};

    fen_pos++;
    fen_pos++;

    while(fen_pos < fen.size() && fen[fen_pos] != ' '){
        ply_100 *= 10;
        ply_100 += (fen[fen_pos++] - '0');
    }

    fen_pos++;

    while(fen_pos < fen.size()){
        ply *= 10;
        ply += (fen[fen_pos++] - '0');
    }

    white_pawn    = is_piece[0];
    white_knight  = is_piece[1];
    white_bishop  = is_piece[2];
    white_rook    = is_piece[3];
    white_queen   = is_piece[4];
    white_king    = is_piece[5];
    black_pawn    = is_piece[6];
    black_knight  = is_piece[7];
    black_bishop  = is_piece[8];
    black_rook    = is_piece[9];
    black_queen  = is_piece[10];
    black_king   = is_piece[11];
    is_white      = is_color[0];
    is_black      = is_color[1];

    update_is_anything_color();
}

void board::print_board() const {
    constexpr std::array<char, 13> parse = {
        'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', ';'
    };

    for(int row = 7; row >= 0; row--){
        cout << row + 1 << ' ';
        for(int column = 0; column < 8; column++){
            int i;
            for(i=0; i<12; i++){
                if(is_piece[i][ind_from_coordinate({row, column})])
                    break;
            }
            cout << parse[i];
        }
        cout << '\n';
    }
    cout << "  abcdefgh\n";

}

bool board::get_turn() const {return turn;}

