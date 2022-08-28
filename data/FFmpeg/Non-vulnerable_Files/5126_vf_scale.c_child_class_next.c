static const AVClass *child_class_next(const AVClass *prev)
{
    return prev ? NULL : sws_get_class();
}
