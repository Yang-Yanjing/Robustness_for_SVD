}
static inline uint32_t align(uint32_t x, uint32_t y) {
    uint32_t mod = x % y;
    if (mod == 0)
        return x;
    else
        return x + y - mod;
}
