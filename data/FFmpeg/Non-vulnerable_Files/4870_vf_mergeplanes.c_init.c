static av_cold int init(AVFilterContext *ctx)
{
    MergePlanesContext *s = ctx->priv;
    int64_t m = s->mapping;
    int i, ret;
    s->outdesc = av_pix_fmt_desc_get(s->out_fmt);
    if (!(s->outdesc->flags & AV_PIX_FMT_FLAG_PLANAR) ||
        s->outdesc->nb_components < 2) {
        av_log(ctx, AV_LOG_ERROR, "Only planar formats with more than one component are supported.\n");
        return AVERROR(EINVAL);
    }
    s->nb_planes = av_pix_fmt_count_planes(s->out_fmt);
    for (i = s->nb_planes - 1; i >= 0; i--) {
        s->map[i][0] = m & 0xf;
        m >>= 4;
        s->map[i][1] = m & 0xf;
        m >>= 4;
        if (s->map[i][0] > 3 || s->map[i][1] > 3) {
            av_log(ctx, AV_LOG_ERROR, "Mapping with out of range input and/or plane number.\n");
            return AVERROR(EINVAL);
        }
        s->nb_inputs = FFMAX(s->nb_inputs, s->map[i][1] + 1);
    }
    av_assert0(s->nb_inputs && s->nb_inputs <= 4);
    for (i = 0; i < s->nb_inputs; i++) {
        AVFilterPad pad = { 0 };
        pad.type = AVMEDIA_TYPE_VIDEO;
        pad.name = av_asprintf("in%d", i);
        if (!pad.name)
            return AVERROR(ENOMEM);
        pad.filter_frame = filter_frame;
        if ((ret = ff_insert_inpad(ctx, i, &pad)) < 0){
            av_freep(&pad.name);
            return ret;
        }
    }
    return 0;
}
