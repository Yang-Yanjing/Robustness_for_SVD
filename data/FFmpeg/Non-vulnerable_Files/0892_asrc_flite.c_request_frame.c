static int request_frame(AVFilterLink *outlink)
{
    AVFrame *samplesref;
    FliteContext *flite = outlink->src->priv;
    int nb_samples = FFMIN(flite->wave_nb_samples, flite->frame_nb_samples);
    if (!nb_samples)
        return AVERROR_EOF;
    samplesref = ff_get_audio_buffer(outlink, nb_samples);
    if (!samplesref)
        return AVERROR(ENOMEM);
    memcpy(samplesref->data[0], flite->wave_samples,
           nb_samples * flite->wave->num_channels * 2);
    samplesref->pts = flite->pts;
    av_frame_set_pkt_pos(samplesref, -1);
    av_frame_set_sample_rate(samplesref, flite->wave->sample_rate);
    flite->pts += nb_samples;
    flite->wave_samples += nb_samples * flite->wave->num_channels;
    flite->wave_nb_samples -= nb_samples;
    return ff_filter_frame(outlink, samplesref);
}
