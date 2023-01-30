static block_t *DecodeBlock (decoder_t *p_dec, block_t **pp_block);
static int Open (vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t *)p_this;
    if (p_dec->fmt_in.i_codec != VLC_CODEC_MIDI)
        return VLC_EGENERIC;
    decoder_sys_t *p_sys = malloc (sizeof (*p_sys));
    if (unlikely(p_sys == NULL))
        return VLC_ENOMEM;
    p_sys->settings = new_fluid_settings ();
    p_sys->synth = new_fluid_synth (p_sys->settings);
    p_sys->soundfont = -1;
    char *font_path = var_InheritString (p_this, "soundfont");
    if (font_path != NULL)
    {
        msg_Dbg (p_this, "loading sound fonts file %s", font_path);
        p_sys->soundfont = fluid_synth_sfload (p_sys->synth, font_path, 1);
        if (p_sys->soundfont == -1)
            msg_Err (p_this, "cannot load sound fonts file %s", font_path);
        free (font_path);
    }
#ifdef _POSIX_VERSION
    else
    {
        glob_t gl;
        glob ("/usr/share/sounds/sf2/*.sf2", GLOB_NOESCAPE, NULL, &gl);
        for (size_t i = 0; i < gl.gl_pathc; i++)
        {
            const char *path = gl.gl_pathv[i];
            msg_Dbg (p_this, "loading sound fonts file %s", path);
            p_sys->soundfont = fluid_synth_sfload (p_sys->synth, path, 1);
            if (p_sys->soundfont != -1)
                break; /* it worked! */
            msg_Err (p_this, "cannot load sound fonts file %s", path);
        }
        globfree (&gl);
    }
#endif
    if (p_sys->soundfont == -1)
    {
        msg_Err (p_this, "sound font file required for synthesis");
        dialog_Fatal (p_this, _("MIDI synthesis not set up"),
            _("A sound font file (.SF2) is required for MIDI synthesis.\n"
              "Please install a sound font and configure it "
              "from the VLC preferences "
              "(Input / Codecs > Audio codecs > FluidSynth).\n"));
        delete_fluid_synth (p_sys->synth);
        delete_fluid_settings (p_sys->settings);
        free (p_sys);
        return VLC_EGENERIC;
    }
    fluid_synth_set_chorus_on (p_sys->synth,
                               var_InheritBool (p_this, "synth-chorus"));
    fluid_synth_set_gain (p_sys->synth,
                          var_InheritFloat (p_this, "synth-gain"));
    fluid_synth_set_polyphony (p_sys->synth,
                               var_InheritInteger (p_this, "synth-polyphony"));
    fluid_synth_set_reverb_on (p_sys->synth,
                               var_InheritBool (p_this, "synth-reverb"));
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.audio.i_rate =
        var_InheritInteger (p_this, "synth-sample-rate");;
    fluid_synth_set_sample_rate (p_sys->synth, p_dec->fmt_out.audio.i_rate);
    p_dec->fmt_out.audio.i_channels = 2;
    p_dec->fmt_out.audio.i_original_channels =
    p_dec->fmt_out.audio.i_physical_channels =
        AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT;
    p_dec->fmt_out.i_codec = VLC_CODEC_FL32;
    p_dec->fmt_out.audio.i_bitspersample = 32;
    date_Init (&p_sys->end_date, p_dec->fmt_out.audio.i_rate, 1);
    date_Set (&p_sys->end_date, 0);
    p_dec->p_sys = p_sys;
    p_dec->pf_decode_audio = DecodeBlock;
    return VLC_SUCCESS;
}
