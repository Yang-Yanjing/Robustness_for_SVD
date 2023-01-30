}
static void OutputSend( sout_stream_t *p_stream, sout_stream_id_sys_t *id, block_t *p_block )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( id->id )
    {
        /* We wait until the first key frame (if needed) and
         * to be beyong i_dts_start (for stream without key frame) */
        if( id->b_wait_key )
        {
            if( p_block->i_flags & BLOCK_FLAG_TYPE_I )
            {
                id->b_wait_key = false;
                id->b_wait_start = false;
            }
            if( ( p_block->i_flags & BLOCK_FLAG_TYPE_MASK ) == 0 )
                id->b_wait_key = false;
        }
        if( id->b_wait_start )
        {
            if( p_block->i_dts >=p_sys->i_dts_start )
                id->b_wait_start = false;
        }
        if( id->b_wait_key || id->b_wait_start )
            block_ChainRelease( p_block );
        else
            sout_StreamIdSend( p_sys->p_out, id->id, p_block );
    }
    else if( p_sys->b_drop )
    {
        block_ChainRelease( p_block );
    }
    else
    {
        size_t i_size;
        block_ChainProperties( p_block, NULL, &i_size, NULL );
        p_sys->i_size += i_size;
        block_ChainLastAppend( &id->pp_last, p_block );
    }
}
