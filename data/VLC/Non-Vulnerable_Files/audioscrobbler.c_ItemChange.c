 *****************************************************************************/
static int ItemChange(vlc_object_t *p_this, const char *psz_var,
                       vlc_value_t oldval, vlc_value_t newval, void *p_data)
{
    intf_thread_t       *p_intf     = (intf_thread_t*) p_data;
    intf_sys_t          *p_sys      = p_intf->p_sys;
    VLC_UNUSED(p_this); VLC_UNUSED(psz_var);
    VLC_UNUSED(oldval); VLC_UNUSED(newval);
    p_sys->b_state_cb       = false;
    p_sys->b_meta_read      = false;
    p_sys->b_submit         = false;
    input_thread_t *p_input = pl_CurrentInput(p_intf);
    if (!p_input || p_input->b_dead)
        return VLC_SUCCESS;
    input_item_t *p_item = input_GetItem(p_input);
    if (!p_item)
    {
        vlc_object_release(p_input);
        return VLC_SUCCESS;
    }
    if (var_CountChoices(p_input, "video-es"))
    {
        msg_Dbg(p_this, "Not an audio-only input, not submitting");
        vlc_object_release(p_input);
        return VLC_SUCCESS;
    }
    p_sys->time_total_pauses = 0;
    time(&p_sys->p_current_song.date);        /* to be sent to last.fm */
    p_sys->p_current_song.i_start = mdate();    /* only used locally */
    var_AddCallback(p_input, "intf-event", PlayingChange, p_intf);
    p_sys->b_state_cb = true;
    if (input_item_IsPreparsed(p_item))
        ReadMetaData(p_intf);
    /* if the input item was not preparsed, we'll do it in PlayingChange()
     * callback, when "state" == PLAYING_S */
    vlc_object_release(p_input);
    return VLC_SUCCESS;
}
