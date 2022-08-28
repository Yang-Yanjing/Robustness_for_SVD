int ff_audio_data_set_channels(AudioData *a, int channels)
{
    if (channels < 1 || channels > AVRESAMPLE_MAX_CHANNELS ||
        channels > a->allocated_channels)
        return AVERROR(EINVAL);
    a->channels  = channels;
    a->planes    = a->is_planar ? channels : 1;
    calc_ptr_alignment(a);
    return 0;
}
