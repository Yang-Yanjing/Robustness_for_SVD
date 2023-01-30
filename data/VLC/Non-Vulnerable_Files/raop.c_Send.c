 *****************************************************************************/
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if ( id->fmt.i_cat == AUDIO_ES && id == p_sys->p_audio_stream )
    {
        /* SendAudio takes care of releasing the buffers */
        SendAudio( p_stream, p_buffer );
    }
    else
    {
        block_ChainRelease( p_buffer );
    }
    return VLC_SUCCESS;
}
