/* We split/pack PCM blocks to a fixed size: p_sys->samples_per_frame * 4 bytes */
static block_t *GetPCM( encoder_t *p_enc, block_t *p_block )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    block_t *p_pcm_block;
    if( !p_block ) goto buffered; /* just return a block if we can */
    /* Put the PCM samples sent by VLC in the Fifo */
    while( p_sys->i_buffer + p_block->i_buffer >= p_sys->samples_per_frame * 4 )
    {
        unsigned int i_buffer = 0;
        p_pcm_block = block_Alloc( p_sys->samples_per_frame * 4 );
        if( !p_pcm_block )
            break;
        if( p_sys->i_buffer )
        {
            memcpy( p_pcm_block->p_buffer, p_sys->p_buffer, p_sys->i_buffer );
            i_buffer = p_sys->i_buffer;
            p_sys->i_buffer = 0;
            free( p_sys->p_buffer );
        }
        memcpy( p_pcm_block->p_buffer + i_buffer,
                    p_block->p_buffer, p_sys->samples_per_frame * 4 - i_buffer );
        p_block->p_buffer += p_sys->samples_per_frame * 4 - i_buffer;
        p_block->i_buffer -= p_sys->samples_per_frame * 4 - i_buffer;
        block_FifoPut( p_sys->p_fifo, p_pcm_block );
    }
    /* We hadn't enough data to make a block, put it in standby */
    if( p_block->i_buffer )
    {
        uint8_t *p_tmp;
        if( p_sys->i_buffer > 0 )
            p_tmp = realloc( p_sys->p_buffer, p_block->i_buffer + p_sys->i_buffer );
        else
            p_tmp = malloc( p_block->i_buffer );
        if( !p_tmp )
        {
            p_sys->i_buffer = 0;
            free( p_sys->p_buffer );
            p_sys->p_buffer = NULL;
            return NULL;
        }
        p_sys->p_buffer = p_tmp;
        memcpy( p_sys->p_buffer + p_sys->i_buffer,
                    p_block->p_buffer, p_block->i_buffer );
        p_sys->i_buffer += p_block->i_buffer;
        p_block->i_buffer = 0;
    }
buffered:
    /* and finally get a block back */
    return block_FifoCount( p_sys->p_fifo ) > 0 ? block_FifoGet( p_sys->p_fifo ) : NULL;
}
