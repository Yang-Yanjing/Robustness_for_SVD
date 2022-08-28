static av_always_inline void get_bits1_vector(GetBitContext *gb, uint8_t *vec,
                                              int elements)
{
    int i;
    for (i = 0; i < elements; i++) {
        vec[i] = get_bits1(gb);
    }
}
