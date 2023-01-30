static void put_s(AVIOContext *bc, int64_t val)
{
    ff_put_v(bc, 2 * FFABS(val) - (val > 0));
}
