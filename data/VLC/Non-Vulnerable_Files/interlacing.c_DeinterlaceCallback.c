}
static int DeinterlaceCallback(vlc_object_t *object, char const *cmd,
                               vlc_value_t oldval, vlc_value_t newval, void *data)
{
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(newval); VLC_UNUSED(data);
    vout_thread_t *vout = (vout_thread_t *)object;
    /* */
    const int  deinterlace_state = var_GetInteger(vout, "deinterlace");
    char       *mode             = var_GetString(vout,  "deinterlace-mode");
    const bool is_needed         = var_GetBool(vout,    "deinterlace-needed");
    if (!mode || !DeinterlaceIsModeValid(mode))
        return VLC_EGENERIC;
    /* */
    char *old = var_CreateGetString(vout, "sout-deinterlace-mode");
    var_SetString(vout, "sout-deinterlace-mode", mode);
    msg_Dbg(vout, "deinterlace %d, mode %s, is_needed %d", deinterlace_state, mode, is_needed);
    if (deinterlace_state == 0 || (deinterlace_state == -1 && !is_needed))
        DeinterlaceRemove(vout);
    else if (!DeinterlaceIsPresent(vout))
        DeinterlaceAdd(vout);
    else if (old && strcmp(old, mode))
        var_TriggerCallback(vout, "video-filter");
    /* */
    free(old);
    free(mode);
    return VLC_SUCCESS;
}
