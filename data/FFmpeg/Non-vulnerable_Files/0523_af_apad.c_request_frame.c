static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    APadContext *s = ctx->priv;
    int ret;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && !ctx->is_disabled) {
        int n_out = s->packet_size;
        AVFrame *outsamplesref;
        if (s->whole_len >= 0 && s->pad_len < 0) {
            s->pad_len = s->pad_len_left = s->whole_len_left;
        }
        if (s->pad_len >=0 || s->whole_len >= 0) {
            n_out = FFMIN(n_out, s->pad_len_left);
            s->pad_len_left -= n_out;
            av_log(ctx, AV_LOG_DEBUG,
                   "padding n_out:%d pad_len_left:%"PRId64"\n", n_out, s->pad_len_left);
        }
        if (!n_out)
            return AVERROR_EOF;
        outsamplesref = ff_get_audio_buffer(outlink, n_out);
        if (!outsamplesref)
            return AVERROR(ENOMEM);
        av_assert0(outsamplesref->sample_rate == outlink->sample_rate);
        av_assert0(outsamplesref->nb_samples  == n_out);
        av_samples_set_silence(outsamplesref->extended_data, 0,
                               n_out,
                               av_frame_get_channels(outsamplesref),
                               outsamplesref->format);
        outsamplesref->pts = s->next_pts;
        if (s->next_pts != AV_NOPTS_VALUE)
            s->next_pts += av_rescale_q(n_out, (AVRational){1, outlink->sample_rate}, outlink->time_base);
        return ff_filter_frame(outlink, outsamplesref);
    }
    return ret;
}
