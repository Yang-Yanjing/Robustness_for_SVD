static int ebml_read_sint(AVIOContext *pb, int size, int64_t *num)
{
    int n = 1;
    if (size > 8)
        return AVERROR_INVALIDDATA;
    if (size == 0) {
        *num = 0;
    } else {
        *num = sign_extend(avio_r8(pb), 8);
        
        while (n++ < size)
            *num = ((uint64_t)*num << 8) | avio_r8(pb);
    }
    return 0;
}
