 */
static void DecoderProcessAudio( decoder_t *p_dec, block_t *p_block, bool b_flush )
{
    decoder_owner_sys_t *p_owner = (decoder_owner_sys_t *)p_dec->p_owner;
    if( p_owner->p_packetizer )
    {
        block_t *p_packetized_block;
        decoder_t *p_packetizer = p_owner->p_packetizer;
        while( (p_packetized_block =
                p_packetizer->pf_packetize( p_packetizer, p_block ? &p_block : NULL )) )
        {
            if( p_packetizer->fmt_out.i_extra && !p_dec->fmt_in.i_extra )
            {
                es_format_Clean( &p_dec->fmt_in );
                es_format_Copy( &p_dec->fmt_in, &p_packetizer->fmt_out );
            }
            while( p_packetized_block )
            {
                block_t *p_next = p_packetized_block->p_next;
                p_packetized_block->p_next = NULL;
                DecoderDecodeAudio( p_dec, p_packetized_block );
                p_packetized_block = p_next;
            }
        }
        /* The packetizer does not output a block that tell the decoder to flush
         * do it ourself */
        if( b_flush )
        {
            block_t *p_null = DecoderBlockFlushNew();
            if( p_null )
                DecoderDecodeAudio( p_dec, p_null );
        }
    }
    else
    {
        DecoderDecodeAudio( p_dec, p_block );
    }
    if( b_flush && p_owner->p_aout )
        aout_DecFlush( p_owner->p_aout );
}
