#include "bitboard.h"

bitboard::bitboard() : value(0) {}
bitboard::bitboard(unsigned long long arg) : value(arg) {}

void bitboard::set_val(bool val, int i) {
    unsigned long long tmp = (1ULL << i);
    if (val) value |= tmp;
    else value &= ~tmp;
}

bool bitboard::operator[](int i) const {
    return (value >> i) & 1;
}

bitboard& bitboard::operator&=(bitboard arg) {
    value &= arg.value;
    return *this;
}

bitboard& bitboard::operator|=(bitboard arg) {
    value |= arg.value;
    return *this;
}

bitboard::operator unsigned long long() const {
    return value;
}

unsigned long long popcount(const bitboard& bb) {
    return std::popcount(bb.value);
}

