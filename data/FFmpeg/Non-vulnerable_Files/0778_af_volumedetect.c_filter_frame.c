static int filter_frame(AVFilterLink *inlink, AVFrame *samples)
{
    AVFilterContext *ctx = inlink->dst;
    VolDetectContext *vd = ctx->priv;
    int64_t layout  = samples->channel_layout;
    int nb_samples  = samples->nb_samples;
    int nb_channels = av_get_channel_layout_nb_channels(layout);
    int nb_planes   = nb_channels;
    int plane, i;
    int16_t *pcm;
    if (!av_sample_fmt_is_planar(samples->format)) {
        nb_samples *= nb_channels;
        nb_planes = 1;
    }
    for (plane = 0; plane < nb_planes; plane++) {
        pcm = (int16_t *)samples->extended_data[plane];
        for (i = 0; i < nb_samples; i++)
            vd->histogram[pcm[i] + 0x8000]++;
    }
    return ff_filter_frame(inlink->dst->outputs[0], samples);
}
