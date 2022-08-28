static int default_query_formats_common(AVFilterContext *ctx,
                                        AVFilterChannelLayouts *(layouts)(void))
{
    int ret;
    enum AVMediaType type = ctx->inputs  && ctx->inputs [0] ? ctx->inputs [0]->type :
                            ctx->outputs && ctx->outputs[0] ? ctx->outputs[0]->type :
                            AVMEDIA_TYPE_VIDEO;
    ret = ff_set_common_formats(ctx, ff_all_formats(type));
    if (ret < 0)
        return ret;
    if (type == AVMEDIA_TYPE_AUDIO) {
        ret = ff_set_common_channel_layouts(ctx, layouts());
        if (ret < 0)
            return ret;
        ret = ff_set_common_samplerates(ctx, ff_all_samplerates());
        if (ret < 0)
            return ret;
    }
    return 0;
}
