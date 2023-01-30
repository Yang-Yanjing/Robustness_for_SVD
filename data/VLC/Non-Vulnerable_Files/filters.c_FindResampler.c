}
static filter_t *FindResampler (vlc_object_t *obj,
                                const audio_sample_format_t *infmt,
                                const audio_sample_format_t *outfmt)
{
    return CreateFilter (obj, "audio resampler", "$audio-resampler", NULL,
                         infmt, outfmt);
}
