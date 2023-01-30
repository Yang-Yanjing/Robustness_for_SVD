
static int64_t log16(uint64_t a)
{
    int i;
    int out = 0;
    if (a < 1 << 16)
        return -log16((1LL << 32) / a);
    a <<= 16;
    for (i = 20; i >= 0; i--) {
        int64_t b = exp16_table[i];
        if (a < (b << 16))
            continue;
        out |= 1 << i;
        a    = ((a / b) << 16) + (((a % b) << 16) + b / 2) / b;
    }
    return out;
}
