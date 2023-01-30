 *****************************************************************************/
static int DemuxDemux( demux_t *p_demux )
{
    block_t *p_block = stream_Block( p_demux->s, DEMUX_BUFFER_SIZE );
    int i_ret;
    if ( p_block == NULL )
        return 0;
    i_ret = HandleSDBuffer( p_demux, p_block->p_buffer, p_block->i_buffer );
    block_Release( p_block );
    return ( i_ret == VLC_SUCCESS );
}
