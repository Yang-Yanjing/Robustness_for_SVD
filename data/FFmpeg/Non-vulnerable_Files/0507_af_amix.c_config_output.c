static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    MixContext *s      = ctx->priv;
    int i;
    char buf[64];
    s->planar          = av_sample_fmt_is_planar(outlink->format);
    s->sample_rate     = outlink->sample_rate;
    outlink->time_base = (AVRational){ 1, outlink->sample_rate };
    s->next_pts        = AV_NOPTS_VALUE;
    s->frame_list = av_mallocz(sizeof(*s->frame_list));
    if (!s->frame_list)
        return AVERROR(ENOMEM);
    s->fifos = av_mallocz_array(s->nb_inputs, sizeof(*s->fifos));
    if (!s->fifos)
        return AVERROR(ENOMEM);
    s->nb_channels = av_get_channel_layout_nb_channels(outlink->channel_layout);
    for (i = 0; i < s->nb_inputs; i++) {
        s->fifos[i] = av_audio_fifo_alloc(outlink->format, s->nb_channels, 1024);
        if (!s->fifos[i])
            return AVERROR(ENOMEM);
    }
    s->input_state = av_malloc(s->nb_inputs);
    if (!s->input_state)
        return AVERROR(ENOMEM);
    memset(s->input_state, INPUT_ON, s->nb_inputs);
    s->active_inputs = s->nb_inputs;
    s->input_scale = av_mallocz_array(s->nb_inputs, sizeof(*s->input_scale));
    if (!s->input_scale)
        return AVERROR(ENOMEM);
    s->scale_norm = s->active_inputs;
    calculate_scales(s, 0);
    av_get_channel_layout_string(buf, sizeof(buf), -1, outlink->channel_layout);
    av_log(ctx, AV_LOG_VERBOSE,
           "inputs:%d fmt:%s srate:%d cl:%s\n", s->nb_inputs,
           av_get_sample_fmt_name(outlink->format), outlink->sample_rate, buf);
    return 0;
}
