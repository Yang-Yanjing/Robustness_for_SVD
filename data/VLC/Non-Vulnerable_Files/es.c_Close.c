 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    free( p_sys->psz_access );
    free( p_sys->psz_access_audio );
    free( p_sys->psz_access_video );
    free( p_sys->psz_mux );
    free( p_sys->psz_mux_audio );
    free( p_sys->psz_mux_video );
    free( p_sys->psz_dst );
    free( p_sys->psz_dst_audio );
    free( p_sys->psz_dst_video );
    free( p_sys );
}
