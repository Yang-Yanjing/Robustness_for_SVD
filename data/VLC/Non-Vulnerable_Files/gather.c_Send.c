 *****************************************************************************/
static int Send( sout_stream_t *p_stream,
                 sout_stream_id_sys_t *id, block_t *p_buffer )
{
    if ( id->b_streamswap )
    {
        id->b_streamswap = false;
        p_buffer->i_flags |= BLOCK_FLAG_DISCONTINUITY;
    }
    return sout_StreamIdSend( p_stream->p_next, id->id, p_buffer );
}
