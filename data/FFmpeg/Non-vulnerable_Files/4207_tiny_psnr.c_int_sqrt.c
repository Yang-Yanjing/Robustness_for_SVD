static uint64_t int_sqrt(uint64_t a)
{
    uint64_t ret    = 0;
    uint64_t ret_sq = 0;
    int s;
    for (s = 31; s >= 0; s--) {
        uint64_t b = ret_sq + (1ULL << (s * 2)) + (ret << s) * 2;
        if (b <= a) {
            ret_sq = b;
            ret   += 1ULL << s;
        }
    }
    return ret;
}
