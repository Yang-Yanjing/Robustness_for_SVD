static int matroska_ebmlnum_sint(MatroskaDemuxContext *matroska,
                                 uint8_t *data, uint32_t size, int64_t *num)
{
    uint64_t unum;
    int res;
    
    if ((res = matroska_ebmlnum_uint(matroska, data, size, &unum)) < 0)
        return res;
    
    *num = unum - ((1LL << (7 * res - 1)) - 1);
    return res;
}
