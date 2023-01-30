AVFILTER_DEFINE_CLASS(concat);
static int query_formats(AVFilterContext *ctx)
{
    ConcatContext *cat = ctx->priv;
    unsigned type, nb_str, idx0 = 0, idx, str, seg;
    AVFilterFormats *formats, *rates = NULL;
    AVFilterChannelLayouts *layouts = NULL;
    for (type = 0; type < TYPE_ALL; type++) {
        nb_str = cat->nb_streams[type];
        for (str = 0; str < nb_str; str++) {
            idx = idx0;
            
            formats = ff_all_formats(type);
            if (!formats)
                return AVERROR(ENOMEM);
            ff_formats_ref(formats, &ctx->outputs[idx]->in_formats);
            if (type == AVMEDIA_TYPE_AUDIO) {
                rates = ff_all_samplerates();
                if (!rates)
                    return AVERROR(ENOMEM);
                ff_formats_ref(rates, &ctx->outputs[idx]->in_samplerates);
                layouts = ff_all_channel_layouts();
                if (!layouts)
                    return AVERROR(ENOMEM);
                ff_channel_layouts_ref(layouts, &ctx->outputs[idx]->in_channel_layouts);
            }
            
            for (seg = 0; seg < cat->nb_segments; seg++) {
                ff_formats_ref(formats, &ctx->inputs[idx]->out_formats);
                if (type == AVMEDIA_TYPE_AUDIO) {
                    ff_formats_ref(rates, &ctx->inputs[idx]->out_samplerates);
                    ff_channel_layouts_ref(layouts, &ctx->inputs[idx]->out_channel_layouts);
                }
                idx += ctx->nb_outputs;
            }
            idx0++;
        }
    }
    return 0;
}
