int av_bprint_finalize(AVBPrint *buf, char **ret_str)
{
    unsigned real_size = FFMIN(buf->len + 1, buf->size);
    char *str;
    int ret = 0;
    if (ret_str) {
        if (av_bprint_is_allocated(buf)) {
            str = av_realloc(buf->str, real_size);
            if (!str)
                str = buf->str;
            buf->str = NULL;
        } else {
            str = av_malloc(real_size);
            if (str)
                memcpy(str, buf->str, real_size);
            else
                ret = AVERROR(ENOMEM);
        }
        *ret_str = str;
    } else {
        if (av_bprint_is_allocated(buf))
            av_freep(&buf->str);
    }
    buf->size = real_size;
    return ret;
}
