static uint64_t get_ts64(GetBitContext *gb, int bits)
{
    if (get_bits_left(gb) < bits)
        return AV_NOPTS_VALUE;
    return get_bits64(gb, bits);
}
