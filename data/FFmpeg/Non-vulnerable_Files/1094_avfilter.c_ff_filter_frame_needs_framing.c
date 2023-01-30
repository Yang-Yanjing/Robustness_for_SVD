static int ff_filter_frame_needs_framing(AVFilterLink *link, AVFrame *frame)
{
    int insamples = frame->nb_samples, inpos = 0, nb_samples;
    AVFrame *pbuf = link->partial_buf;
    int nb_channels = av_frame_get_channels(frame);
    int ret = 0;
    link->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    
    while (insamples) {
        if (!pbuf) {
            AVRational samples_tb = { 1, link->sample_rate };
            pbuf = ff_get_audio_buffer(link, link->partial_buf_size);
            if (!pbuf) {
                av_log(link->dst, AV_LOG_WARNING,
                       "Samples dropped due to memory allocation failure.\n");
                return 0;
            }
            av_frame_copy_props(pbuf, frame);
            pbuf->pts = frame->pts;
            if (pbuf->pts != AV_NOPTS_VALUE)
                pbuf->pts += av_rescale_q(inpos, samples_tb, link->time_base);
            pbuf->nb_samples = 0;
        }
        nb_samples = FFMIN(insamples,
                           link->partial_buf_size - pbuf->nb_samples);
        av_samples_copy(pbuf->extended_data, frame->extended_data,
                        pbuf->nb_samples, inpos,
                        nb_samples, nb_channels, link->format);
        inpos                   += nb_samples;
        insamples               -= nb_samples;
        pbuf->nb_samples += nb_samples;
        if (pbuf->nb_samples >= link->min_samples) {
            ret = ff_filter_frame_framed(link, pbuf);
            pbuf = NULL;
        }
    }
    av_frame_free(&frame);
    link->partial_buf = pbuf;
    return ret;
}
