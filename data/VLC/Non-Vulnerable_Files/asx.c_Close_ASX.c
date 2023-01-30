 *****************************************************************************/
void Close_ASX( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    free( p_sys->psz_prefix );
    free( p_sys->psz_data );
    free( p_sys );
}
