static const AVClass *filter_child_class_next(const AVClass *prev)
{
    const AVFilter *f = NULL;
    
    while (prev && (f = avfilter_next(f)))
        if (f->priv_class == prev)
            break;
    
    if (prev && !f)
        return NULL;
    
    while ((f = avfilter_next(f)))
        if (f->priv_class)
            return f->priv_class;
    return NULL;
}
