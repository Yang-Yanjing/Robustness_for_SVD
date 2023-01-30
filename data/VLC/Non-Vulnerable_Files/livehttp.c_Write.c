 *****************************************************************************/
static ssize_t Write( sout_access_out_t *p_access, block_t *p_buffer )
{
    size_t i_write = 0;
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    block_t *p_temp;
    while( p_buffer )
    {
        if( ( p_sys->b_splitanywhere  || ( p_buffer->i_flags & BLOCK_FLAG_HEADER ) ) )
        {
            if( unlikely( CheckSegmentChange( p_access, p_buffer ) != VLC_SUCCESS ) )
            {
                block_ChainRelease ( p_buffer );
                return -1;
            }
            ssize_t writevalue = writeSegment( p_access );
            if( unlikely( writevalue < 0 ) )
            {
                block_ChainRelease ( p_buffer );
                return -1;
            }
            p_sys->b_segment_has_data = true;
            i_write += writevalue;
        }
        p_temp = p_buffer->p_next;
        p_buffer->p_next = NULL;
        block_ChainLastAppend( &p_sys->last_block_buffer, p_buffer );
        p_buffer = p_temp;
    }
    return i_write;
}
