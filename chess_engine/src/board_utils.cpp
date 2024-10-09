#include "board_utils.h"

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
}
