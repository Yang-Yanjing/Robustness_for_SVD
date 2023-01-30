static subpicture_t *Convert( decoder_t *, block_t * );
static subpicture_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    if( pp_block && *pp_block )
    {
        Push( p_dec, *pp_block );
        *pp_block = NULL;
    }
    for( ;; )
    {
        block_t *p_block = Pop( p_dec );
        if( !p_block )
            break;
        subpicture_t *p_spu = Convert( p_dec, p_block );
        if( p_spu )
            return p_spu;
    }
    return NULL;
}
