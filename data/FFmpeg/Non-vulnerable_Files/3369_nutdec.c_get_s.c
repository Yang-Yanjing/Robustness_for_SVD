static int64_t get_s(AVIOContext *bc)
{
    int64_t v = ffio_read_varlen(bc) + 1;
    if (v & 1)
        return -(v >> 1);
    else
        return  (v >> 1);
}
