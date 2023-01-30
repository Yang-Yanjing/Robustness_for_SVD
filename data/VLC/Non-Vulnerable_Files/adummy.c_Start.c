}
static int Start(audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    if (AOUT_FMT_SPDIF(fmt) && var_InheritBool(aout, "spdif"))
    {
        fmt->i_format = VLC_CODEC_SPDIFL;
        fmt->i_bytes_per_frame = AOUT_SPDIF_SIZE;
        fmt->i_frame_length = A52_FRAME_NB;
    }
    else
        fmt->i_format = HAVE_FPU ? VLC_CODEC_FL32 : VLC_CODEC_S16N;
    return VLC_SUCCESS;
}
