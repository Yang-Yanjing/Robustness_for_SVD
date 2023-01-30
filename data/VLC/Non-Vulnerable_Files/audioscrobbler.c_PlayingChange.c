 *****************************************************************************/
static int PlayingChange(vlc_object_t *p_this, const char *psz_var,
                       vlc_value_t oldval, vlc_value_t newval, void *p_data)
{
    VLC_UNUSED(oldval);
    intf_thread_t   *p_intf = (intf_thread_t*) p_data;
    intf_sys_t      *p_sys  = p_intf->p_sys;
    input_thread_t  *p_input = (input_thread_t*)p_this;
    int             state;
    VLC_UNUSED(psz_var);
    if (newval.i_int != INPUT_EVENT_STATE) return VLC_SUCCESS;
    if (var_CountChoices(p_input, "video-es"))
    {
        msg_Dbg(p_this, "Not an audio-only input, not submitting");
        return VLC_SUCCESS;
    }
    state = var_GetInteger(p_input, "state");
    if (!p_sys->b_meta_read && state >= PLAYING_S)
    {
        ReadMetaData(p_intf);
        return VLC_SUCCESS;
    }
    if (state >= END_S)
        AddToQueue(p_intf);
    else if (state == PAUSE_S)
        p_sys->time_pause = mdate();
    else if (p_sys->time_pause > 0 && state == PLAYING_S)
    {
        p_sys->time_total_pauses += (mdate() - p_sys->time_pause);
        p_sys->time_pause = 0;
    }
    return VLC_SUCCESS;
}
