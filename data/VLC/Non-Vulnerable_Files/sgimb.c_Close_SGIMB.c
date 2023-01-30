 *****************************************************************************/
void Close_SGIMB( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    free( p_sys->psz_uri );
    free( p_sys->psz_server );
    free( p_sys->psz_location );
    free( p_sys->psz_name );
    free( p_sys->psz_user );
    free( p_sys->psz_password );
    free( p_sys->psz_mcast_ip );
    free( p_demux->p_sys );
    return;
}
