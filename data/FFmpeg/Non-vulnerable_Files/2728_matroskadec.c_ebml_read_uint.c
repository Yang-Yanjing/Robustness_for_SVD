static int ebml_read_uint(AVIOContext *pb, int size, uint64_t *num)
{
    int n = 0;
    if (size > 8)
        return AVERROR_INVALIDDATA;
    
    *num = 0;
    while (n++ < size)
        *num = (*num << 8) | avio_r8(pb);
    return 0;
}
