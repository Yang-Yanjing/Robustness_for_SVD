 */
int aout_OutputNew (audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    aout_OutputAssertLocked (aout);
    /* Ideally, the audio filters would be created before the audio output,
     * and the ideal audio format would be the output of the filters chain.
     * But that scheme would not really play well with digital pass-through. */
    if (AOUT_FMT_LINEAR(fmt))
    {   /* Try to stay in integer domain if possible for no/slow FPU. */
        fmt->i_format = (fmt->i_bitspersample > 16) ? VLC_CODEC_FL32
                                                    : VLC_CODEC_S16N;
        aout_FormatPrepare (fmt);
    }
    if (aout->start (aout, fmt))
    {
        msg_Err (aout, "module not functional");
        return -1;
    }
    if (!var_Type (aout, "stereo-mode"))
    {
        var_Create (aout, "stereo-mode",
                    VLC_VAR_INTEGER | VLC_VAR_HASCHOICE | VLC_VAR_DOINHERIT);
        vlc_value_t txt;
        txt.psz_string = _("Stereo audio mode");
        var_Change (aout, "stereo-mode", VLC_VAR_SETTEXT, &txt, NULL);
    }
    /* The user may have selected a different channels configuration. */
    var_AddCallback (aout, "stereo-mode", aout_ChannelsRestart, NULL);
    switch (var_GetInteger (aout, "stereo-mode"))
    {
        case AOUT_VAR_CHAN_RSTEREO:
            fmt->i_original_channels |= AOUT_CHAN_REVERSESTEREO;
            break;
        case AOUT_VAR_CHAN_STEREO:
            fmt->i_original_channels = AOUT_CHANS_STEREO;
            break;
        case AOUT_VAR_CHAN_LEFT:
            fmt->i_original_channels = AOUT_CHAN_LEFT;
            break;
        case AOUT_VAR_CHAN_RIGHT:
            fmt->i_original_channels = AOUT_CHAN_RIGHT;
            break;
        case AOUT_VAR_CHAN_DOLBYS:
            fmt->i_original_channels = AOUT_CHANS_STEREO|AOUT_CHAN_DOLBYSTEREO;
            break;
        default:
        {
            if ((fmt->i_original_channels & AOUT_CHAN_PHYSMASK)
                                                          != AOUT_CHANS_STEREO)
                 break;
            vlc_value_t val, txt;
            val.i_int = 0;
            var_Change (aout, "stereo-mode", VLC_VAR_DELCHOICE, &val, NULL);
            if (fmt->i_original_channels & AOUT_CHAN_DOLBYSTEREO)
            {
                val.i_int = AOUT_VAR_CHAN_DOLBYS;
                txt.psz_string = _("Dolby Surround");
            }
            else
            {
                val.i_int = AOUT_VAR_CHAN_STEREO;
                txt.psz_string = _("Stereo");
            }
            var_Change (aout, "stereo-mode", VLC_VAR_ADDCHOICE, &val, &txt);
            var_Change (aout, "stereo-mode", VLC_VAR_SETVALUE, &val, NULL);
            val.i_int = AOUT_VAR_CHAN_LEFT;
            txt.psz_string = _("Left");
            var_Change (aout, "stereo-mode", VLC_VAR_ADDCHOICE, &val, &txt);
            if (fmt->i_original_channels & AOUT_CHAN_DUALMONO)
            {   /* Go directly to the left channel. */
                fmt->i_original_channels = AOUT_CHAN_LEFT;
                var_Change (aout, "stereo-mode", VLC_VAR_SETVALUE, &val, NULL);
            }
            val.i_int = AOUT_VAR_CHAN_RIGHT;
            txt.psz_string = _("Right");
            var_Change (aout, "stereo-mode", VLC_VAR_ADDCHOICE, &val, &txt);
            val.i_int = AOUT_VAR_CHAN_RSTEREO;
            txt.psz_string = _("Reverse stereo");
            var_Change (aout, "stereo-mode", VLC_VAR_ADDCHOICE, &val, &txt);
        }
    }
    aout_FormatPrepare (fmt);
    aout_FormatPrint (aout, "output", fmt);
    return 0;
}
