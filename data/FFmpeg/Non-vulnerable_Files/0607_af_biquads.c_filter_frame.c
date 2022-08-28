static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    BiquadsContext *s       = inlink->dst->priv;
    AVFilterLink *outlink   = inlink->dst->outputs[0];
    AVFrame *out_buf;
    int nb_samples = buf->nb_samples;
    int ch;
    if (av_frame_is_writable(buf)) {
        out_buf = buf;
    } else {
        out_buf = ff_get_audio_buffer(inlink, nb_samples);
        if (!out_buf)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out_buf, buf);
    }
    for (ch = 0; ch < av_frame_get_channels(buf); ch++)
        s->filter(buf->extended_data[ch],
                  out_buf->extended_data[ch], nb_samples,
                  &s->cache[ch].i1, &s->cache[ch].i2,
                  &s->cache[ch].o1, &s->cache[ch].o2,
                  s->b0, s->b1, s->b2, s->a1, s->a2);
    if (buf != out_buf)
        av_frame_free(&buf);
    return ff_filter_frame(outlink, out_buf);
}
