 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( p_sys->p_out )
        sout_StreamChainDelete( p_sys->p_out, p_sys->p_out );
    TAB_CLEAN( p_sys->i_id, p_sys->id );
    free( p_sys->psz_prefix );
    free( p_sys );
}
