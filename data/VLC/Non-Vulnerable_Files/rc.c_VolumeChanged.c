 ********************************************************************/
static int VolumeChanged( vlc_object_t *p_this, char const *psz_cmd,
    vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    (void) p_this;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(newval);
    intf_thread_t *p_intf = (intf_thread_t*)p_data;
    vlc_mutex_lock( &p_intf->p_sys->status_lock );
    msg_rc( STATUS_CHANGE "( audio volume: %ld )",
            lroundf(newval.f_float * AOUT_VOLUME_DEFAULT) );
    vlc_mutex_unlock( &p_intf->p_sys->status_lock );
    return VLC_SUCCESS;
}
