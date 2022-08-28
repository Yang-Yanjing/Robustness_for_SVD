static const AVClass *resample_child_class_next(const AVClass *prev)
{
    return prev ? NULL : swr_get_class();
}
