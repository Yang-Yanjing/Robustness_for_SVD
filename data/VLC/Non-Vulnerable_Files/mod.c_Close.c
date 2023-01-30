 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    ModPlug_Unload( p_sys->f );
    free( p_sys->p_data );
    free( p_sys );
}
