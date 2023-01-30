}
static int UrlsChange( vlc_object_t *p_this, char const *psz_var,
                       vlc_value_t oldval, vlc_value_t newval,
                       void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(psz_var); VLC_UNUSED(oldval);
    VLC_UNUSED(newval);
    services_discovery_sys_t *p_sys  = (services_discovery_sys_t *)p_data;
    vlc_mutex_lock( &p_sys->lock );
    p_sys->b_update = true;
    p_sys->update_type = UPDATE_URLS;
    vlc_cond_signal( &p_sys->wait );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
