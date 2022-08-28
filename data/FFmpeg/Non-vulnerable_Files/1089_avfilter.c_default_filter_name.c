static const char *default_filter_name(void *filter_ctx)
{
    AVFilterContext *ctx = filter_ctx;
    return ctx->name ? ctx->name : ctx->filter->name;
}
