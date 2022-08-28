static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    ShowWavesContext *showwaves = ctx->priv;
    int nb_channels = inlink->channels;
    if (!showwaves->n)
        showwaves->n = FFMAX(1, ((double)inlink->sample_rate / (showwaves->w * av_q2d(showwaves->rate))) + 0.5);
    showwaves->buf_idx = 0;
    if (!(showwaves->buf_idy = av_mallocz_array(nb_channels, sizeof(*showwaves->buf_idy)))) {
        av_log(ctx, AV_LOG_ERROR, "Could not allocate showwaves buffer\n");
        return AVERROR(ENOMEM);
    }
    outlink->w = showwaves->w;
    outlink->h = showwaves->h;
    outlink->sample_aspect_ratio = (AVRational){1,1};
    outlink->frame_rate = av_div_q((AVRational){inlink->sample_rate,showwaves->n},
                                   (AVRational){showwaves->w,1});
    av_log(ctx, AV_LOG_VERBOSE, "s:%dx%d r:%f n:%d\n",
           showwaves->w, showwaves->h, av_q2d(outlink->frame_rate), showwaves->n);
    return 0;
}
