 */
static void DecoderProcess( decoder_t *p_dec, block_t *p_block )
{
    decoder_owner_sys_t *p_owner = (decoder_owner_sys_t *)p_dec->p_owner;
    const bool b_flush_request = p_block && (p_block->i_flags & BLOCK_FLAG_CORE_FLUSH);
    if( p_dec->b_error )
    {
        if( p_block )
            block_Release( p_block );
        goto flush;
    }
    if( p_block && p_block->i_buffer <= 0 )
    {
        assert( !b_flush_request );
        block_Release( p_block );
        return;
    }
#ifdef ENABLE_SOUT
    if( p_owner->b_packetizer )
    {
        if( p_block )
            p_block->i_flags &= ~BLOCK_FLAG_CORE_PRIVATE_MASK;
        DecoderProcessSout( p_dec, p_block );
    }
    else
#endif
    {
        bool b_flush = false;
        if( p_block )
        {
            const bool b_flushing = p_owner->i_preroll_end == INT64_MAX;
            DecoderUpdatePreroll( &p_owner->i_preroll_end, p_block );
            b_flush = !b_flushing && b_flush_request;
            p_block->i_flags &= ~BLOCK_FLAG_CORE_PRIVATE_MASK;
        }
        if( p_dec->fmt_out.i_cat == AUDIO_ES )
        {
            DecoderProcessAudio( p_dec, p_block, b_flush );
        }
        else if( p_dec->fmt_out.i_cat == VIDEO_ES )
        {
            DecoderProcessVideo( p_dec, p_block, b_flush );
        }
        else if( p_dec->fmt_out.i_cat == SPU_ES )
        {
            DecoderProcessSpu( p_dec, p_block, b_flush );
        }
        else
        {
            msg_Err( p_dec, "unknown ES format" );
            p_dec->b_error = true;
        }
    }
    /* */
flush:
    if( b_flush_request )
        DecoderProcessOnFlush( p_dec );
}
