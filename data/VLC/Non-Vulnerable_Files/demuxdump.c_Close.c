 */
static void Close( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t*)p_this;
    sout_access_out_t *out = (void *)p_demux->p_sys;
    sout_AccessOutDelete( out );
}
