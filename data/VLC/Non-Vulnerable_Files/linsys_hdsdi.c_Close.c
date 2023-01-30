 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    CloseCapture( p_demux );
    free( p_sys );
}
