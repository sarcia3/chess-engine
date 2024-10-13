#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include <utility> 
#include <string>

namespace board_utils {
    int ind_from_coordinate(const std::pair<int, int> &coordinate);

    bool coordinate_is_legal(const std::pair<int, int> &coordinate);

    bool ind_is_legal(int ind);

    std::pair<int, int> gen_coordinate(int i);

    std::string move_readable(const std::pair<int, int> &arg);
}

#endif
