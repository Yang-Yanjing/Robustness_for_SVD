static int ebml_read_ascii(AVIOContext *pb, int size, char **str)
{
    char *res;
    

    if (!(res = av_malloc(size + 1)))
        return AVERROR(ENOMEM);
    if (avio_read(pb, (uint8_t *) res, size) != size) {
        av_free(res);
        return AVERROR(EIO);
    }
    (res)[size] = '\0';
    av_free(*str);
    *str = res;
    return 0;
}
