int ff_audio_data_realloc(AudioData *a, int nb_samples)
{
    int ret, new_buf_size, plane_size, p;
    
    if (a->allocated_samples >= nb_samples)
        return 0;
    
    if (a->read_only || !a->allow_realloc)
        return AVERROR(EINVAL);
    new_buf_size = av_samples_get_buffer_size(&plane_size,
                                              a->allocated_channels, nb_samples,
                                              a->sample_fmt, 0);
    if (new_buf_size < 0)
        return new_buf_size;
    


    if (a->nb_samples > 0 && a->is_planar) {
        uint8_t *new_data[AVRESAMPLE_MAX_CHANNELS] = { NULL };
        ret = av_samples_alloc(new_data, &plane_size, a->allocated_channels,
                               nb_samples, a->sample_fmt, 0);
        if (ret < 0)
            return ret;
        for (p = 0; p < a->planes; p++)
            memcpy(new_data[p], a->data[p], a->nb_samples * a->stride);
        av_freep(&a->buffer);
        memcpy(a->data, new_data, sizeof(new_data));
        a->buffer = a->data[0];
    } else {
        av_freep(&a->buffer);
        a->buffer = av_malloc(new_buf_size);
        if (!a->buffer)
            return AVERROR(ENOMEM);
        ret = av_samples_fill_arrays(a->data, &plane_size, a->buffer,
                                     a->allocated_channels, nb_samples,
                                     a->sample_fmt, 0);
        if (ret < 0)
            return ret;
    }
    a->buffer_size       = new_buf_size;
    a->allocated_samples = nb_samples;
    calc_ptr_alignment(a);
    a->samples_align = plane_size / a->stride;
    return 0;
}
