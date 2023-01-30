static uint64_t key_shift_left(uint64_t CDn) {
    uint64_t carries = (CDn >> 27) & 0x10000001;
    CDn <<= 1;
    CDn &= ~0x10000001;
    CDn |= carries;
    return CDn;
}
