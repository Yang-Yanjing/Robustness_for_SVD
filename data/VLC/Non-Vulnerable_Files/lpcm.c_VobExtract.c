}
static void VobExtract( block_t *p_aout_buffer, block_t *p_block,
                        unsigned i_bits )
{
    /* 20/24 bits LPCM use special packing */
    if( i_bits == 24 )
    {
        uint32_t *p_out = (uint32_t *)p_aout_buffer->p_buffer;
        while( p_block->i_buffer / 12 )
        {
            /* Sample 1 */
            *(p_out++) = (p_block->p_buffer[ 0] << 24)
                       | (p_block->p_buffer[ 1] << 16)
                       | (p_block->p_buffer[ 8] <<  8);
            /* Sample 2 */
            *(p_out++) = (p_block->p_buffer[ 2] << 24)
                       | (p_block->p_buffer[ 3] << 16)
                       | (p_block->p_buffer[ 9] <<  8);
            /* Sample 3 */
            *(p_out++) = (p_block->p_buffer[ 4] << 24)
                       | (p_block->p_buffer[ 5] << 16)
                       | (p_block->p_buffer[10] <<  8);
            /* Sample 4 */
            *(p_out++) = (p_block->p_buffer[ 6] << 24)
                       | (p_block->p_buffer[ 7] << 16)
                       | (p_block->p_buffer[11] <<  8);
            p_block->i_buffer -= 12;
            p_block->p_buffer += 12;
        }
    }
    else if( i_bits == 20 )
    {
        uint32_t *p_out = (uint32_t *)p_aout_buffer->p_buffer;
        while( p_block->i_buffer / 10 )
        {
            /* Sample 1 */
            *(p_out++) = ( p_block->p_buffer[0]         << 24)
                       | ( p_block->p_buffer[1]         << 16)
                       | ((p_block->p_buffer[8] & 0xF0) <<  8);
            /* Sample 2 */
            *(p_out++) = ( p_block->p_buffer[2]         << 24)
                       | ( p_block->p_buffer[3]         << 16)
                       | ((p_block->p_buffer[8] & 0x0F) << 12);
            /* Sample 3 */
            *(p_out++) = ( p_block->p_buffer[4]         << 24)
                       | ( p_block->p_buffer[5]         << 16)
                       | ((p_block->p_buffer[9] & 0xF0) <<  8);
            /* Sample 4 */
            *(p_out++) = ( p_block->p_buffer[6]         << 24)
                       | ( p_block->p_buffer[7]         << 16)
                       | ((p_block->p_buffer[9] & 0x0F) << 12);
            p_block->i_buffer -= 10;
            p_block->p_buffer += 10;
        }
    }
    else
    {
        assert( i_bits == 16 );
#ifdef WORDS_BIGENDIAN
        memcpy( p_aout_buffer->p_buffer, p_block->p_buffer, p_block->i_buffer );
#else
        swab( p_block->p_buffer, p_aout_buffer->p_buffer, p_block->i_buffer );
#endif
    }
}
