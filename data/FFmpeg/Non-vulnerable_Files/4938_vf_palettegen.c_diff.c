static av_always_inline int diff(const uint32_t a, const uint32_t b)
{
    const uint8_t c1[] = {a >> 16 & 0xff, a >> 8 & 0xff, a & 0xff};
    const uint8_t c2[] = {b >> 16 & 0xff, b >> 8 & 0xff, b & 0xff};
    const int dr = c1[0] - c2[0];
    const int dg = c1[1] - c2[1];
    const int db = c1[2] - c2[2];
    return dr*dr + dg*dg + db*db;
}
