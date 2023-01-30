 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    es_out_Del( p_demux->out, p_sys->es );
    free( p_sys->fmt.p_extra );
    free( p_sys );
}
