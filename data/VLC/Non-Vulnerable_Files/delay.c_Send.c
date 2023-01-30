}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    if ( id == p_sys->id )
    {
        block_t *p_block = p_buffer;
        while ( p_block != NULL )
        {
            if ( p_block->i_pts && p_block->i_pts != VLC_TS_INVALID )
                p_block->i_pts += p_sys->i_delay;
            if ( p_block->i_dts && p_block->i_dts != VLC_TS_INVALID )
                p_block->i_dts += p_sys->i_delay;
            p_block = p_block->p_next;
        }
    }
    return p_stream->p_next->pf_send( p_stream->p_next, id, p_buffer );
}
