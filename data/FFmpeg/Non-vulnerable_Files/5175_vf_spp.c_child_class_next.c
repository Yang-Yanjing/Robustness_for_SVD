static const AVClass *child_class_next(const AVClass *prev)
{
    return prev ? NULL : avcodec_dct_get_class();
}
