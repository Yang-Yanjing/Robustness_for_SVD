 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    services_discovery_t *p_sd = ( services_discovery_t * )p_this;
    free( p_sd->p_sys->psz_name );
    vlc_cancel( p_sd->p_sys->thread );
    vlc_join( p_sd->p_sys->thread, NULL );
    free( p_sd->p_sys );
}
