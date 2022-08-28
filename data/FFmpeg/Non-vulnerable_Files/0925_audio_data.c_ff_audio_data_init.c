int ff_audio_data_init(AudioData *a, uint8_t **src, int plane_size, int channels,
                       int nb_samples, enum AVSampleFormat sample_fmt,
                       int read_only, const char *name)
{
    int p;
    memset(a, 0, sizeof(*a));
    a->class = &audio_data_class;
    if (channels < 1 || channels > AVRESAMPLE_MAX_CHANNELS) {
        av_log(a, AV_LOG_ERROR, "invalid channel count: %d\n", channels);
        return AVERROR(EINVAL);
    }
    a->sample_size = av_get_bytes_per_sample(sample_fmt);
    if (!a->sample_size) {
        av_log(a, AV_LOG_ERROR, "invalid sample format\n");
        return AVERROR(EINVAL);
    }
    a->is_planar = ff_sample_fmt_is_planar(sample_fmt, channels);
    a->planes    = a->is_planar ? channels : 1;
    a->stride    = a->sample_size * (a->is_planar ? 1 : channels);
    for (p = 0; p < (a->is_planar ? channels : 1); p++) {
        if (!src[p]) {
            av_log(a, AV_LOG_ERROR, "invalid NULL pointer for src[%d]\n", p);
            return AVERROR(EINVAL);
        }
        a->data[p] = src[p];
    }
    a->allocated_samples  = nb_samples * !read_only;
    a->nb_samples         = nb_samples;
    a->sample_fmt         = sample_fmt;
    a->channels           = channels;
    a->allocated_channels = channels;
    a->read_only          = read_only;
    a->allow_realloc      = 0;
    a->name               = name ? name : "{no name}";
    calc_ptr_alignment(a);
    a->samples_align = plane_size / a->stride;
    return 0;
}
