}
static int Start(audio_output_t *p_aout, audio_sample_format_t *restrict fmt)
{
    struct aout_sys_t *p_sys = NULL;
    p_sys = p_aout->sys;
    p_sys->au_component = NULL;
    p_sys->au_unit = NULL;
    p_sys->i_bytes_per_sample = 0;
    aout_FormatPrint(p_aout, "VLC is looking for:", fmt);
    if (StartAnalog(p_aout, fmt)) {
        msg_Dbg(p_aout, "analog AudioUnit output successfully opened");
        p_aout->play = Play;
        p_aout->flush = Flush;
        p_aout->time_get = TimeGet;
        p_aout->pause = Pause;
        return VLC_SUCCESS;
    }
    /* If we reach this, this aout has failed */
    msg_Err(p_aout, "opening AudioUnit output failed");
    return VLC_EGENERIC;
}
