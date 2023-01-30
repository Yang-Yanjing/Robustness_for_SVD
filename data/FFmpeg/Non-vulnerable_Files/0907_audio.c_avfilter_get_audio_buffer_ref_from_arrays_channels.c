FF_DISABLE_DEPRECATION_WARNINGS
AVFilterBufferRef* avfilter_get_audio_buffer_ref_from_arrays_channels(uint8_t **data,
                                                                      int linesize,int perms,
                                                                      int nb_samples,
                                                                      enum AVSampleFormat sample_fmt,
                                                                      int channels,
                                                                      uint64_t channel_layout)
{
    int planes;
    AVFilterBuffer    *samples    = av_mallocz(sizeof(*samples));
    AVFilterBufferRef *samplesref = av_mallocz(sizeof(*samplesref));
    if (!samples || !samplesref)
        goto fail;
    av_assert0(channels);
    av_assert0(channel_layout == 0 ||
               channels == av_get_channel_layout_nb_channels(channel_layout));
    samplesref->buf         = samples;
    samplesref->buf->free   = ff_avfilter_default_free_buffer;
    if (!(samplesref->audio = av_mallocz(sizeof(*samplesref->audio))))
        goto fail;
    samplesref->audio->nb_samples     = nb_samples;
    samplesref->audio->channel_layout = channel_layout;
    samplesref->audio->channels       = channels;
    planes = av_sample_fmt_is_planar(sample_fmt) ? channels : 1;
    
    samplesref->perms = perms | AV_PERM_READ;
    samples->refcount  = 1;
    samplesref->type   = AVMEDIA_TYPE_AUDIO;
    samplesref->format = sample_fmt;
    memcpy(samples->data, data,
           FFMIN(FF_ARRAY_ELEMS(samples->data), planes)*sizeof(samples->data[0]));
    memcpy(samplesref->data, samples->data, sizeof(samples->data));
    samples->linesize[0] = samplesref->linesize[0] = linesize;
    if (planes > FF_ARRAY_ELEMS(samples->data)) {
        samples->   extended_data = av_mallocz_array(sizeof(*samples->extended_data),
                                               planes);
        samplesref->extended_data = av_mallocz_array(sizeof(*samplesref->extended_data),
                                               planes);
        if (!samples->extended_data || !samplesref->extended_data)
            goto fail;
        memcpy(samples->   extended_data, data, sizeof(*data)*planes);
        memcpy(samplesref->extended_data, data, sizeof(*data)*planes);
    } else {
        samples->extended_data    = samples->data;
        samplesref->extended_data = samplesref->data;
    }
    samplesref->pts = AV_NOPTS_VALUE;
    return samplesref;
fail:
    if (samples && samples->extended_data != samples->data)
        av_freep(&samples->extended_data);
    if (samplesref) {
        av_freep(&samplesref->audio);
        if (samplesref->extended_data != samplesref->data)
            av_freep(&samplesref->extended_data);
    }
    av_freep(&samplesref);
    av_freep(&samples);
    return NULL;
}
