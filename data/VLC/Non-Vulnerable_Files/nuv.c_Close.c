 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    demux_t        *p_demux = (demux_t*)p_this;
    demux_sys_t    *p_sys = p_demux->p_sys;
    free( p_sys->p_extra_f );
    demux_IndexClean( &p_sys->idx );
    free( p_sys );
}
