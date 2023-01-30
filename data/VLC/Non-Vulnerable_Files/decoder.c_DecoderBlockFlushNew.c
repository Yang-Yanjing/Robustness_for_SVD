}
static block_t *DecoderBlockFlushNew()
{
    block_t *p_null = block_Alloc( 128 );
    if( !p_null )
        return NULL;
    p_null->i_flags |= BLOCK_FLAG_DISCONTINUITY |
                       BLOCK_FLAG_CORRUPTED |
                       BLOCK_FLAG_CORE_FLUSH;
    memset( p_null->p_buffer, 0, p_null->i_buffer );
    return p_null;
}
