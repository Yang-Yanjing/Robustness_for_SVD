static int av_bprint_alloc(AVBPrint *buf, unsigned room)
{
    char *old_str, *new_str;
    unsigned min_size, new_size;
    if (buf->size == buf->size_max)
        return AVERROR(EIO);
    if (!av_bprint_is_complete(buf))
        return AVERROR_INVALIDDATA; 
    min_size = buf->len + 1 + FFMIN(UINT_MAX - buf->len - 1, room);
    new_size = buf->size > buf->size_max / 2 ? buf->size_max : buf->size * 2;
    if (new_size < min_size)
        new_size = FFMIN(buf->size_max, min_size);
    old_str = av_bprint_is_allocated(buf) ? buf->str : NULL;
    new_str = av_realloc(old_str, new_size);
    if (!new_str)
        return AVERROR(ENOMEM);
    if (!old_str)
        memcpy(new_str, buf->str, buf->len + 1);
    buf->str  = new_str;
    buf->size = new_size;
    return 0;
}
