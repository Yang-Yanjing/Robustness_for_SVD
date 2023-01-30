static int vs2av_log(int type, const char *tag, const char *format, ...)
{
    va_list ap;
    VS2AVLogCtx ctx;
    AVClass class = {
        .class_name = tag,
        .item_name  = av_default_item_name,
        .option     = 0,
        .version    = LIBAVUTIL_VERSION_INT,
        .category   = AV_CLASS_CATEGORY_FILTER,
    };
    ctx.class = &class;
    va_start(ap, format);
    av_vlog(&ctx, type, format, ap);
    va_end(ap);
    return VS_OK;
}
