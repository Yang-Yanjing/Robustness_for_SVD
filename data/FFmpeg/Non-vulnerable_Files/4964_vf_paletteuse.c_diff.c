static av_always_inline int diff(const uint8_t *c1, const uint8_t *c2)
{
    
    const int dr = c1[0] - c2[0];
    const int dg = c1[1] - c2[1];
    const int db = c1[2] - c2[2];
    return dr*dr + dg*dg + db*db;
}
