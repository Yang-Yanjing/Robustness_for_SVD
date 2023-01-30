 ****************************************************************************/
static void *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    FILE *stream = (void *)p_dec->p_sys;
    block_t *p_block;
    if( !pp_block || !*pp_block ) return NULL;
    p_block = *pp_block;
    if( stream != NULL
     && p_block->i_buffer > 0
     && !(p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED)) )
    {
        fwrite( p_block->p_buffer, 1, p_block->i_buffer, stream );
        msg_Dbg( p_dec, "dumped %zu bytes", p_block->i_buffer );
    }
    block_Release( p_block );
    return NULL;
}
