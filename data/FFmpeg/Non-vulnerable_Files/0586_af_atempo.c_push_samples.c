static int push_samples(ATempoContext *atempo,
                        AVFilterLink *outlink,
                        int n_out)
{
    int ret;
    atempo->dst_buffer->sample_rate = outlink->sample_rate;
    atempo->dst_buffer->nb_samples  = n_out;
    
    atempo->dst_buffer->pts =
        av_rescale_q(atempo->nsamples_out,
                     (AVRational){ 1, outlink->sample_rate },
                     outlink->time_base);
    ret = ff_filter_frame(outlink, atempo->dst_buffer);
    atempo->dst_buffer = NULL;
    atempo->dst        = NULL;
    atempo->dst_end    = NULL;
    if (ret < 0)
        return ret;
    atempo->nsamples_out += n_out;
    return 0;
}
