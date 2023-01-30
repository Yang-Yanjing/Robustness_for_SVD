 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys  = p_demux->p_sys;
    var_Destroy( p_demux, "rawdv-hurry-up");
    free( p_sys );
}
