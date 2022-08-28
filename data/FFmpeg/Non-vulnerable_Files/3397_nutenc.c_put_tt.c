static void put_tt(NUTContext *nut, AVRational *time_base, AVIOContext *bc, uint64_t val)
{
    val *= nut->time_base_count;
    val += time_base - nut->time_base;
    ff_put_v(bc, val);
}
