 */
static int Demux( demux_t *p_demux )
{
    sout_access_out_t *out = (void *)p_demux->p_sys;
    block_t *block = block_Alloc( DUMP_BLOCKSIZE );
    if( unlikely(block == NULL) )
        return -1;
    int rd = stream_Read( p_demux->s, block->p_buffer, DUMP_BLOCKSIZE );
    if ( rd <= 0 )
    {
        block_Release( block );
        return rd;
    }
    block->i_buffer = rd;
    size_t wr = sout_AccessOutWrite( out, block );
    if( wr != (size_t)rd )
    {
        msg_Err( p_demux, "cannot write data" );
        return -1;
    }
    return 1;
}
