static int send_silence(AVFilterContext *ctx, unsigned in_no, unsigned out_no,
                        int64_t seg_delta)
{
    ConcatContext *cat = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[out_no];
    int64_t base_pts = cat->in[in_no].pts + cat->delta_ts - seg_delta;
    int64_t nb_samples, sent = 0;
    int frame_nb_samples, ret;
    AVRational rate_tb = { 1, ctx->inputs[in_no]->sample_rate };
    AVFrame *buf;
    int nb_channels = av_get_channel_layout_nb_channels(outlink->channel_layout);
    if (!rate_tb.den)
        return AVERROR_BUG;
    nb_samples = av_rescale_q(seg_delta - cat->in[in_no].pts,
                              outlink->time_base, rate_tb);
    frame_nb_samples = FFMAX(9600, rate_tb.den / 5); 
    while (nb_samples) {
        frame_nb_samples = FFMIN(frame_nb_samples, nb_samples);
        buf = ff_get_audio_buffer(outlink, frame_nb_samples);
        if (!buf)
            return AVERROR(ENOMEM);
        av_samples_set_silence(buf->extended_data, 0, frame_nb_samples,
                               nb_channels, outlink->format);
        buf->pts = base_pts + av_rescale_q(sent, rate_tb, outlink->time_base);
        ret = ff_filter_frame(outlink, buf);
        if (ret < 0)
            return ret;
        sent       += frame_nb_samples;
        nb_samples -= frame_nb_samples;
    }
    return 0;
}
