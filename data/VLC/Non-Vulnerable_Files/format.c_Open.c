static cvt_t FindConversion(vlc_fourcc_t src, vlc_fourcc_t dst);
static int Open(vlc_object_t *object)
{
    filter_t     *filter = (filter_t *)object;
    const es_format_t *src = &filter->fmt_in;
    es_format_t       *dst = &filter->fmt_out;
    if (!AOUT_FMTS_SIMILAR(&src->audio, &dst->audio))
        return VLC_EGENERIC;
    if (src->i_codec == dst->i_codec)
        return VLC_EGENERIC;
    filter->pf_audio_filter = FindConversion(src->i_codec, dst->i_codec);
    if (filter->pf_audio_filter == NULL)
        return VLC_EGENERIC;
    msg_Dbg(filter, "%4.4s->%4.4s, bits per sample: %i->%i",
            (char *)&src->i_codec, (char *)&dst->i_codec,
            src->audio.i_bitspersample, dst->audio.i_bitspersample);
    return VLC_SUCCESS;
}
