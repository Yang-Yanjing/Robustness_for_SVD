 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    services_discovery_t *p_sd = ( services_discovery_t * )p_this;
    services_discovery_sys_t *p_sys;
    if( !( p_sys = malloc( sizeof( services_discovery_sys_t ) ) ) )
        return VLC_ENOMEM;
    p_sd->p_sys = p_sys;
    p_sys->psz_name = NULL;
    vlc_mutex_lock( &mtp_lock );
    if( !b_mtp_initialized )
    {
        LIBMTP_Init();
        b_mtp_initialized = true;
    }
    vlc_mutex_unlock( &mtp_lock );
    if (vlc_clone (&p_sys->thread, Run, p_sd, VLC_THREAD_PRIORITY_LOW))
    {
        free (p_sys);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
