static int query_formats(AVFilterContext *ctx)
{
    BufferSourceContext *c = ctx->priv;
    AVFilterChannelLayouts *channel_layouts = NULL;
    AVFilterFormats *formats = NULL;
    AVFilterFormats *samplerates = NULL;
    switch (ctx->outputs[0]->type) {
    case AVMEDIA_TYPE_VIDEO:
        ff_add_format(&formats, c->pix_fmt);
        ff_set_common_formats(ctx, formats);
        break;
    case AVMEDIA_TYPE_AUDIO:
        ff_add_format(&formats,           c->sample_fmt);
        ff_set_common_formats(ctx, formats);
        ff_add_format(&samplerates,       c->sample_rate);
        ff_set_common_samplerates(ctx, samplerates);
        ff_add_channel_layout(&channel_layouts,
                              c->channel_layout ? c->channel_layout :
                              FF_COUNT2LAYOUT(c->channels));
        ff_set_common_channel_layouts(ctx, channel_layouts);
        break;
    default:
        return AVERROR(EINVAL);
    }
    return 0;
}
