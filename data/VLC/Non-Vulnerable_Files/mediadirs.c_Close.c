 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    services_discovery_t *p_sd = (services_discovery_t *)p_this;
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    vlc_cancel( p_sys->thread );
    vlc_join( p_sys->thread, NULL );
    var_DelCallback( p_sd->p_libvlc, p_sys->psz_var, onNewFileAdded, p_sd );
    free( p_sys->psz_dir[1] );
    free( p_sys->psz_dir[0] );
    free( p_sys );
}
