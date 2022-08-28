int avresample_set_compensation(AVAudioResampleContext *avr, int sample_delta,
                                int compensation_distance)
{
    ResampleContext *c;
    AudioData *fifo_buf = NULL;
    int ret = 0;
    if (compensation_distance < 0)
        return AVERROR(EINVAL);
    if (!compensation_distance && sample_delta)
        return AVERROR(EINVAL);
    if (!avr->resample_needed) {
#if FF_API_RESAMPLE_CLOSE_OPEN
        

        int fifo_samples;
        int restore_matrix = 0;
        double matrix[AVRESAMPLE_MAX_CHANNELS * AVRESAMPLE_MAX_CHANNELS] = { 0 };
        
        fifo_samples = av_audio_fifo_size(avr->out_fifo);
        if (fifo_samples > 0) {
            fifo_buf = ff_audio_data_alloc(avr->out_channels, fifo_samples,
                                           avr->out_sample_fmt, NULL);
            if (!fifo_buf)
                return AVERROR(EINVAL);
            ret = ff_audio_data_read_from_fifo(avr->out_fifo, fifo_buf,
                                               fifo_samples);
            if (ret < 0)
                goto reinit_fail;
        }
        
        if (avr->am) {
            ret = avresample_get_matrix(avr, matrix, AVRESAMPLE_MAX_CHANNELS);
            if (ret < 0)
                goto reinit_fail;
            restore_matrix = 1;
        }
        
        avresample_close(avr);
        avr->force_resampling = 1;
        
        if (restore_matrix) {
            ret = avresample_set_matrix(avr, matrix, AVRESAMPLE_MAX_CHANNELS);
            if (ret < 0)
                goto reinit_fail;
        }
        
        ret = avresample_open(avr);
        if (ret < 0)
            goto reinit_fail;
        
        if (fifo_samples > 0) {
            ret = ff_audio_data_add_to_fifo(avr->out_fifo, fifo_buf, 0,
                                            fifo_samples);
            if (ret < 0)
                goto reinit_fail;
            ff_audio_data_free(&fifo_buf);
        }
#else
        av_log(avr, AV_LOG_ERROR, "Unable to set resampling compensation\n");
        return AVERROR(EINVAL);
#endif
    }
    c = avr->resample;
    c->compensation_distance = compensation_distance;
    if (compensation_distance) {
        c->dst_incr = c->ideal_dst_incr - c->ideal_dst_incr *
                      (int64_t)sample_delta / compensation_distance;
    } else {
        c->dst_incr = c->ideal_dst_incr;
    }
    return 0;
reinit_fail:
    ff_audio_data_free(&fifo_buf);
    return ret;
}
