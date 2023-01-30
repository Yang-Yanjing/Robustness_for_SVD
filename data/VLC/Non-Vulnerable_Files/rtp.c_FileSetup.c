****************************************************************************/
static int FileSetup( sout_stream_t *p_stream )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    FILE            *f;
    if( p_sys->psz_sdp == NULL )
        return VLC_EGENERIC; /* too early */
    if( ( f = vlc_fopen( p_sys->psz_sdp_file, "wt" ) ) == NULL )
    {
        msg_Err( p_stream, "cannot open file '%s' (%s)",
                 p_sys->psz_sdp_file, vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    fputs( p_sys->psz_sdp, f );
    fclose( f );
    return VLC_SUCCESS;
}
