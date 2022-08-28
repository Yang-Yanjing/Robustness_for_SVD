static int dither_value(int p)
{
    const int q = p ^ (p >> 3);
    return   (p & 4) >> 2 | (q & 4) >> 1 \
           | (p & 2) << 1 | (q & 2) << 2 \
           | (p & 1) << 4 | (q & 1) << 5;
}
