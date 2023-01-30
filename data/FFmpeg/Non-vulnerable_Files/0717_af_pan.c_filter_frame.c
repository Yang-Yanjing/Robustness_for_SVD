static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    int ret;
    int n = insamples->nb_samples;
    AVFilterLink *const outlink = inlink->dst->outputs[0];
    AVFrame *outsamples = ff_get_audio_buffer(outlink, n);
    PanContext *pan = inlink->dst->priv;
    if (!outsamples)
        return AVERROR(ENOMEM);
    swr_convert(pan->swr, outsamples->extended_data, n,
                (void *)insamples->extended_data, n);
    av_frame_copy_props(outsamples, insamples);
    outsamples->channel_layout = outlink->channel_layout;
    av_frame_set_channels(outsamples, outlink->channels);
    ret = ff_filter_frame(outlink, outsamples);
    av_frame_free(&insamples);
    return ret;
}
