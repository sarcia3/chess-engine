#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include <utility> 

namespace board_utils {
    int ind_from_coordinate(const std::pair<int, int> &coordinate);

    bool coordinate_is_legal(const std::pair<int, int> &coordinate);

    bool ind_is_legal(int ind);

    std::pair<int, int> gen_coordinate(int i);
}

#endif