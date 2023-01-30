 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t *p_stream = (sout_stream_t *)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if ( !p_sys->b_done ) Finish( p_stream );
    chromaprint_free( p_sys->p_chromaprint_ctx );
    free( p_sys );
}
