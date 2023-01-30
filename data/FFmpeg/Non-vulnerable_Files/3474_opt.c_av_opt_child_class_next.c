const AVClass *av_opt_child_class_next(const AVClass *parent, const AVClass *prev)
{
    if (parent->child_class_next)
        return parent->child_class_next(prev);
    return NULL;
}
