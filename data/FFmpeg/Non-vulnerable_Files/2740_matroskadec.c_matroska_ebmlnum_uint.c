static int matroska_ebmlnum_uint(MatroskaDemuxContext *matroska,
                                 uint8_t *data, uint32_t size, uint64_t *num)
{
    AVIOContext pb;
    ffio_init_context(&pb, data, size, 0, NULL, NULL, NULL, NULL);
    return ebml_read_num(matroska, &pb, FFMIN(size, 8), num);
}
