 *****************************************************************************/
static void Close(vlc_object_t *p_this)
{
    intf_thread_t               *p_intf = (intf_thread_t*) p_this;
    intf_sys_t                  *p_sys  = p_intf->p_sys;
    var_DelCallback(pl_Get(p_intf), "activity", ItemChange, p_intf);
    vlc_cancel(p_sys->thread);
    vlc_join(p_sys->thread, NULL);
    input_thread_t *p_input = pl_CurrentInput(p_intf);
    if (p_input)
    {
        if (p_sys->b_state_cb)
            var_DelCallback(p_input, "intf-event", PlayingChange, p_intf);
        vlc_object_release(p_input);
    }
    int i;
    for (i = 0; i < p_sys->i_songs; i++)
        DeleteSong(&p_sys->p_queue[i]);
    vlc_UrlClean(&p_sys->p_submit_url);
#if 0 //NOT USED
    free(p_sys->psz_nowp_host);
    free(p_sys->psz_nowp_file);
#endif
    vlc_cond_destroy(&p_sys->wait);
    vlc_mutex_destroy(&p_sys->lock);
    free(p_sys);
}
