static int get_sae(uint8_t *src, int ref, int stride)
{
    int x,y;
    int acc = 0;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            acc += FFABS(src[x + y * stride] - ref);
        }
    }
    return acc;
}
