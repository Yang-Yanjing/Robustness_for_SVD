 *****************************************************************************/
static void DemuxClose( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;
    demux_sys_t *p_sys   = p_demux->p_sys;
    free( p_sys->psz_telx );
    free( p_sys->psz_telx_lang );
    free( p_sys );
}
