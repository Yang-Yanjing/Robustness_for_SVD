 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    if( p_sys->p_es )  block_Release( p_sys->p_es );
    if( p_sys->p_pes ) block_Release( p_sys->p_pes );
    free( p_sys );
}
