}
static filter_t *FindConverter (vlc_object_t *obj,
                                const audio_sample_format_t *infmt,
                                const audio_sample_format_t *outfmt)
{
    return CreateFilter (obj, "audio converter", NULL, NULL, infmt, outfmt);
}
