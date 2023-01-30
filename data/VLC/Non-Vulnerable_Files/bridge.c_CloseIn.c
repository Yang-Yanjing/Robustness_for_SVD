 *****************************************************************************/
static void CloseIn( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    in_sout_stream_sys_t *p_sys = (in_sout_stream_sys_t *)p_stream->p_sys;
    free( p_sys->psz_name );
    free( p_sys );
}
