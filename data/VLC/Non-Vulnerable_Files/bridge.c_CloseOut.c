 *****************************************************************************/
static void CloseOut( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    out_sout_stream_sys_t *p_sys = (out_sout_stream_sys_t *)p_stream->p_sys;
    free( p_sys->psz_name );
    free( p_sys );
}
