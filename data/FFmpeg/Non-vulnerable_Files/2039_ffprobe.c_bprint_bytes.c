static void bprint_bytes(AVBPrint *bp, const uint8_t *ubuf, size_t ubuf_size)
{
    int i;
    av_bprintf(bp, "0X");
    for (i = 0; i < ubuf_size; i++)
        av_bprintf(bp, "%02X", ubuf[i]);
}
