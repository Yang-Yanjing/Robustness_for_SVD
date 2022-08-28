static const AVClass *resample_child_class_next(const AVClass *prev)
{
    return prev ? NULL : avresample_get_class();
}
