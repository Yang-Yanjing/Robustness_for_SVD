AudioData *ff_audio_data_alloc(int channels, int nb_samples,
                               enum AVSampleFormat sample_fmt, const char *name)
{
    AudioData *a;
    int ret;
    if (channels < 1 || channels > AVRESAMPLE_MAX_CHANNELS)
        return NULL;
    a = av_mallocz(sizeof(*a));
    if (!a)
        return NULL;
    a->sample_size = av_get_bytes_per_sample(sample_fmt);
    if (!a->sample_size) {
        av_free(a);
        return NULL;
    }
    a->is_planar = ff_sample_fmt_is_planar(sample_fmt, channels);
    a->planes    = a->is_planar ? channels : 1;
    a->stride    = a->sample_size * (a->is_planar ? 1 : channels);
    a->class              = &audio_data_class;
    a->sample_fmt         = sample_fmt;
    a->channels           = channels;
    a->allocated_channels = channels;
    a->read_only          = 0;
    a->allow_realloc      = 1;
    a->name               = name ? name : "{no name}";
    if (nb_samples > 0) {
        ret = ff_audio_data_realloc(a, nb_samples);
        if (ret < 0) {
            av_free(a);
            return NULL;
        }
        return a;
    } else {
        calc_ptr_alignment(a);
        return a;
    }
}
