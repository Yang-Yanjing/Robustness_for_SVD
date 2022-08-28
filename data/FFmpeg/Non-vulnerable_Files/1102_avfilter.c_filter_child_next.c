static void *filter_child_next(void *obj, void *prev)
{
    AVFilterContext *ctx = obj;
    if (!prev && ctx->filter && ctx->filter->priv_class && ctx->priv)
        return ctx->priv;
    return NULL;
}
