static int request_frame(AVFilterLink *outlink)
{
    int ret;
    ANullContext *null = outlink->src->priv;
    AVFrame *samplesref;
    samplesref = ff_get_audio_buffer(outlink, null->nb_samples);
    if (!samplesref)
        return AVERROR(ENOMEM);
    samplesref->pts = null->pts;
    samplesref->channel_layout = null->channel_layout;
    samplesref->sample_rate = outlink->sample_rate;
    ret = ff_filter_frame(outlink, av_frame_clone(samplesref));
    av_frame_free(&samplesref);
    if (ret < 0)
        return ret;
    null->pts += null->nb_samples;
    return ret;
}
