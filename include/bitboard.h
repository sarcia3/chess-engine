#ifndef BITBOARD_H
#define BITBOARD_H

#include <bit>

class bitboard {
    unsigned long long value;

public:
    bitboard();
    bitboard(unsigned long long arg);

    void set_val(bool val, int i);
    bool operator[](int i) const;
    int get_first_one() const;

    bitboard& operator&=(bitboard arg);
    bitboard& operator|=(bitboard arg);
    operator unsigned long long() const;

    friend unsigned long long popcount(const bitboard& bb);
};

#endif 

