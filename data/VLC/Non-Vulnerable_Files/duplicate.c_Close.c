 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    int i;
    msg_Dbg( p_stream, "closing a duplication" );
    for( i = 0; i < p_sys->i_nb_streams; i++ )
    {
        sout_StreamChainDelete(p_sys->pp_streams[i], p_sys->pp_last_streams[i]);
        free( p_sys->ppsz_select[i] );
    }
    free( p_sys->pp_streams );
    free( p_sys->pp_last_streams );
    free( p_sys->ppsz_select );
    free( p_sys );
}
