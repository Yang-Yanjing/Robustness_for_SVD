 */
audio_output_t *aout_New (vlc_object_t *parent)
{
    vlc_value_t val, text;
    audio_output_t *aout = vlc_custom_create (parent, sizeof (aout_instance_t),
                                              "audio output");
    if (unlikely(aout == NULL))
        return NULL;
    aout_owner_t *owner = aout_owner (aout);
    vlc_mutex_init (&owner->lock);
    vlc_mutex_init (&owner->req.lock);
    vlc_mutex_init (&owner->dev.lock);
    owner->req.device = (char *)unset_str;
    owner->req.volume = -1.f;
    owner->req.mute = -1;
    vlc_object_set_destructor (aout, aout_Destructor);
    /* Audio output module callbacks */
    var_Create (aout, "volume", VLC_VAR_FLOAT);
    var_AddCallback (aout, "volume", var_Copy, parent);
    var_Create (aout, "mute", VLC_VAR_BOOL | VLC_VAR_DOINHERIT);
    var_AddCallback (aout, "mute", var_Copy, parent);
    var_Create (aout, "device", VLC_VAR_STRING);
    aout->event.volume_report = aout_VolumeNotify;
    aout->event.mute_report = aout_MuteNotify;
    aout->event.policy_report = aout_PolicyNotify;
    aout->event.device_report = aout_DeviceNotify;
    aout->event.hotplug_report = aout_HotplugNotify;
    aout->event.gain_request = aout_GainNotify;
    aout->event.restart_request = aout_RestartNotify;
    /* Audio output module initialization */
    aout->start = NULL;
    aout->stop = NULL;
    aout->volume_set = NULL;
    aout->mute_set = NULL;
    aout->device_select = NULL;
    owner->module = module_need (aout, "audio output", "$aout", false);
    if (owner->module == NULL)
    {
        msg_Err (aout, "no suitable audio output module");
        vlc_object_release (aout);
        return NULL;
    }
    /*
     * Persistent audio output variables
     */
    module_config_t *cfg;
    char *str;
    /* Visualizations */
    var_Create (aout, "visual", VLC_VAR_STRING | VLC_VAR_HASCHOICE);
    text.psz_string = _("Visualizations");
    var_Change (aout, "visual", VLC_VAR_SETTEXT, &text, NULL);
    val.psz_string = (char *)"";
    text.psz_string = _("Disable");
    var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    val.psz_string = (char *)"spectrometer";
    text.psz_string = _("Spectrometer");
    var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    val.psz_string = (char *)"scope";
    text.psz_string = _("Scope");
    var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    val.psz_string = (char *)"spectrum";
    text.psz_string = _("Spectrum");
    var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    val.psz_string = (char *)"vuMeter";
    text.psz_string = _("Vu meter");
    var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    /* Look for goom plugin */
    if (module_exists ("goom"))
    {
        val.psz_string = (char *)"goom";
        text.psz_string = (char *)"Goom";
        var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    }
#ifndef _WIN32
    /* Look for libprojectM plugin */
    if (module_exists ("projectm"))
    {
        val.psz_string = (char *)"projectm";
        text.psz_string = (char*)"projectM";
        var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    }
    /* Look for VSXu plugin */
    if (module_exists ("vsxu"))
    {
        val.psz_string = (char *)"vsxu";
        text.psz_string = (char*)"Vovoid VSXu";
        var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    }
    /* Look for glspectrum plugin */
    if (module_exists ("glspectrum"))
    {
        val.psz_string = (char *)"glspectrum";
        text.psz_string = (char*)"3D spectrum";
        var_Change (aout, "visual", VLC_VAR_ADDCHOICE, &val, &text);
    }
#endif
    str = var_GetNonEmptyString (aout, "effect-list");
    if (str != NULL)
    {
        var_SetString (aout, "visual", str);
        free (str);
    }
    var_Create (aout, "audio-filter", VLC_VAR_STRING | VLC_VAR_DOINHERIT);
    var_AddCallback (aout, "audio-filter", FilterCallback, NULL);
    text.psz_string = _("Audio filters");
    var_Change (aout, "audio-filter", VLC_VAR_SETTEXT, &text, NULL);
    var_Create (aout, "audio-visual", VLC_VAR_STRING | VLC_VAR_DOINHERIT);
    text.psz_string = _("Audio visualizations");
    var_Change (aout, "audio-visual", VLC_VAR_SETTEXT, &text, NULL);
    /* Replay gain */
    var_Create (aout, "audio-replay-gain-mode",
                VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    text.psz_string = _("Replay gain");
    var_Change (aout, "audio-replay-gain-mode", VLC_VAR_SETTEXT, &text, NULL);
    cfg = config_FindConfig (VLC_OBJECT(aout), "audio-replay-gain-mode");
    if (likely(cfg != NULL))
        for (unsigned i = 0; i < cfg->list_count; i++)
        {
            val.psz_string = cfg->list.psz[i];
            text.psz_string = vlc_gettext(cfg->list_text[i]);
            var_Change (aout, "audio-replay-gain-mode", VLC_VAR_ADDCHOICE,
                            &val, &text);
        }
    /* Equalizer */
    var_Create (aout, "equalizer-preamp", VLC_VAR_FLOAT | VLC_VAR_DOINHERIT);
    var_Create (aout, "equalizer-bands", VLC_VAR_STRING | VLC_VAR_DOINHERIT);
    var_Create (aout, "equalizer-preset", VLC_VAR_STRING | VLC_VAR_DOINHERIT);
    return aout;
}
