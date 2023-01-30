}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buf )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if ( p_sys->id != id )
    {
        /* drop the whole buffer at once */
        block_ChainRelease( p_buf );
        return VLC_SUCCESS;
    }
    while( p_buf )
    {
        block_t *p_next;
        int i_samples = p_buf->i_buffer / (BYTESPERSAMPLE * id->i_channels);
        p_sys->i_total_samples += i_samples;
        if ( !p_sys->b_finished && id->i_samples > 0 && p_buf->i_buffer )
        {
            if(! chromaprint_feed( p_sys->p_chromaprint_ctx,
                                   p_buf->p_buffer,
                                   p_buf->i_buffer / BYTESPERSAMPLE ) )
                msg_Warn( p_stream, "feed error" );
            id->i_samples -= i_samples;
            if ( id->i_samples < 1 && !p_sys->b_finished )
            {
                p_sys->b_finished = true;
                msg_Dbg( p_stream, "Fingerprint collection finished" );
            }
        }
        p_next = p_buf->p_next;
        block_Release( p_buf );
        p_buf = p_next;
    }
    return VLC_SUCCESS;
}
