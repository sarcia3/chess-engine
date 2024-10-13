#include "board_utils.h"

using namespace std;

namespace board_utils {

    int ind_from_coordinate(const std::pair<int, int> &coordinate) {
        return coordinate.first * 8 + coordinate.second;
    }

    bool coordinate_is_legal(const std::pair<int, int> &coordinate) {
        return coordinate.first >= 0 && coordinate.first < 8 &&
               coordinate.second >= 0 && coordinate.second < 8;
    }

    bool ind_is_legal(int ind) {
        return ind >= 0 && ind < 64;
    }

    std::pair<int, int> gen_coordinate(int i) {
        return {i/8, i%8};
    }

    string move_readable(const pair<int, int> &arg){
        const auto &top = arg;
        if(top.first == top.second){
            if(top.first % 2 == 0) return "o-o";
            else return "o-o-o";
        }
    
        if(top.first<0) {
            string without_prom = (std::string(1, char(gen_coordinate(-top.first).second + 'a')) + 
                      std::to_string(gen_coordinate(-top.first).first + 1) + '-' +
                      std::string(1, char(gen_coordinate(top.second >> 2).second + 'a')) + 
                      std::to_string(gen_coordinate(top.second >> 2).first + 1) + '=');
                      
            if(top.second%4 == 0){
                without_prom+=("N");
                return without_prom;
            }
            if(top.second%4 == 1){
                without_prom+=("B");
                return without_prom;
            }
            if(top.second%4 == 2){
                without_prom+=("R");
                return without_prom;
            }
            if(top.second%4 == 3){
                without_prom+=("Q");
                return without_prom;
            }
        }
        string move = (std::string(1, char(gen_coordinate(top.first).second + 'a')) + 
                      std::to_string(gen_coordinate(top.first).first + 1) + '-' +
                      std::string(1, char(gen_coordinate(top.second).second + 'a')) + 
                      std::to_string(gen_coordinate(top.second).first + 1));
    
        return move;
    }
}
