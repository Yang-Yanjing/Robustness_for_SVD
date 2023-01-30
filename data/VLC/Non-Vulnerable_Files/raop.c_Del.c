 *****************************************************************************/
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    int i_err = VLC_SUCCESS;
    if ( p_sys->p_audio_stream == id )
        p_sys->p_audio_stream = NULL;
    FreeId( id );
    return i_err;
}
