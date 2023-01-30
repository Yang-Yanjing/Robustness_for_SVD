}
static void DecoderFlush( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_assert_locked( &p_owner->lock );
    /* Empty the fifo */
    block_FifoEmpty( p_owner->p_fifo );
    p_owner->b_waiting = false;
    /* Monitor for flush end */
    p_owner->b_flushing = true;
    vlc_cond_signal( &p_owner->wait_request );
    /* Send a special block */
    block_t *p_null = DecoderBlockFlushNew();
    if( !p_null )
        return;
    input_DecoderDecode( p_dec, p_null, false );
    /* */
    while( p_owner->b_flushing )
        vlc_cond_wait( &p_owner->wait_acknowledge, &p_owner->lock );
}
