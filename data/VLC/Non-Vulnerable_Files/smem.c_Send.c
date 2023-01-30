}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    if ( id->format->i_cat == VIDEO_ES )
        return SendVideo( p_stream, id, p_buffer );
    else if ( id->format->i_cat == AUDIO_ES )
        return SendAudio( p_stream, id, p_buffer );
    return VLC_SUCCESS;
}
