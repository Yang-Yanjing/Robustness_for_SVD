 ****************************************************************************/
static int SapSetup( sout_stream_t *p_stream )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* Remove the previous session */
    if( p_sys->p_session != NULL)
    {
        sout_AnnounceUnRegister( p_stream, p_sys->p_session);
        p_sys->p_session = NULL;
    }
    if( p_sys->i_es > 0 && p_sys->psz_sdp && *p_sys->psz_sdp )
        p_sys->p_session = sout_AnnounceRegisterSDP( p_stream,
                                                     p_sys->psz_sdp,
                                                     p_sys->psz_destination );
    return VLC_SUCCESS;
}
