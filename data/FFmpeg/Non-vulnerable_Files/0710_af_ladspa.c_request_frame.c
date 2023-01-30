static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    LADSPAContext *s = ctx->priv;
    AVFrame *out;
    int64_t t;
    int i;
    if (ctx->nb_inputs)
        return ff_request_frame(ctx->inputs[0]);
    t = av_rescale(s->pts, AV_TIME_BASE, s->sample_rate);
    if (s->duration >= 0 && t >= s->duration)
        return AVERROR_EOF;
    out = ff_get_audio_buffer(outlink, s->nb_samples);
    if (!out)
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_outputs; i++)
        s->desc->connect_port(s->handles[0], s->opmap[i],
                (LADSPA_Data*)out->extended_data[i]);
    s->desc->run(s->handles[0], s->nb_samples);
    for (i = 0; i < s->nb_outputcontrols; i++)
        print_ctl_info(ctx, AV_LOG_INFO, s, i, s->ocmap, s->octlv, 1);
    out->sample_rate = s->sample_rate;
    out->pts         = s->pts;
    s->pts          += s->nb_samples;
    return ff_filter_frame(outlink, out);
}
