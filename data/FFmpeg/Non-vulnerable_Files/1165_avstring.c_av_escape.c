int av_escape(char **dst, const char *src, const char *special_chars,
              enum AVEscapeMode mode, int flags)
{
    AVBPrint dstbuf;
    av_bprint_init(&dstbuf, 1, AV_BPRINT_SIZE_UNLIMITED);
    av_bprint_escape(&dstbuf, src, special_chars, mode, flags);
    if (!av_bprint_is_complete(&dstbuf)) {
        av_bprint_finalize(&dstbuf, NULL);
        return AVERROR(ENOMEM);
    } else {
        av_bprint_finalize(&dstbuf, dst);
        return dstbuf.len;
    }
}
