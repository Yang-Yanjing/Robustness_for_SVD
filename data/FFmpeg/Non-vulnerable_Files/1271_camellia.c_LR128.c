const int av_camellia_size = sizeof(AVCAMELLIA);
static void LR128(uint64_t d[2], const uint64_t K[2], int x)
{
    int i = 0;
    if (64 <= x && x < 128) {
        i = 1;
        x -= 64;
    }
    if (x <= 0 || x >= 128) {
        d[0] = K[i];
        d[1] = K[!i];
        return;
    }
    d[0] = (K[i] << x | K[!i] >> (64 - x));
    d[1] = (K[!i] << x | K[i] >> (64 - x));
}
