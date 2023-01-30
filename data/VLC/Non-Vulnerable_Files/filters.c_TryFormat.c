}
static filter_t *TryFormat (vlc_object_t *obj, vlc_fourcc_t codec,
                            audio_sample_format_t *restrict fmt)
{
    audio_sample_format_t output = *fmt;
    assert (codec != fmt->i_format);
    output.i_format = codec;
    aout_FormatPrepare (&output);
    filter_t *filter = FindConverter (obj, fmt, &output);
    if (filter != NULL)
        *fmt = output;
    return filter;
}
