/** Callback for visualization selection */
static int VisualizationCallback (vlc_object_t *obj, const char *var,
                                  vlc_value_t oldval, vlc_value_t newval,
                                  void *data)
{
    const char *mode = newval.psz_string;
    if (!*mode)
        mode = "none";
    /* FIXME: This ugly hack enforced by visual effect-list, as is the need for
     * separate "visual" (external) and "audio-visual" (internal) variables...
     * The visual plugin should have one submodule per effect instead. */
    if (strcasecmp (mode, "none") && strcasecmp (mode, "goom")
     && strcasecmp (mode, "projectm") && strcasecmp (mode, "vsxu")
     && strcasecmp (mode, "glspectrum"))
    {
        var_Create (obj, "effect-list", VLC_VAR_STRING);
        var_SetString (obj, "effect-list", mode);
        mode = "visual";
    }
    var_SetString (obj, "audio-visual", mode);
    aout_InputRequestRestart ((audio_output_t *)obj);
    (void) var; (void) oldval; (void) data;
    return VLC_SUCCESS;
}
