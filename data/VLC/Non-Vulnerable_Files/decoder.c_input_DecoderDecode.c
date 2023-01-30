 */
void input_DecoderDecode( decoder_t *p_dec, block_t *p_block, bool b_do_pace )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    if( b_do_pace )
    {
        /* The fifo is not consumed when waiting and so will
         * deadlock vlc.
         * There is no need to lock as b_waiting is never modified
         * inside decoder thread. */
        if( !p_owner->b_waiting )
            block_FifoPace( p_owner->p_fifo, 10, SIZE_MAX );
    }
#ifdef __arm__
    else if( block_FifoSize( p_owner->p_fifo ) > 50*1024*1024 /* 50 MiB */ )
#else
    else if( block_FifoSize( p_owner->p_fifo ) > 400*1024*1024 /* 400 MiB, ie ~ 50mb/s for 60s */ )
#endif
    {
        /* FIXME: ideally we would check the time amount of data
         * in the FIFO instead of its size. */
        msg_Warn( p_dec, "decoder/packetizer fifo full (data not "
                  "consumed quickly enough), resetting fifo!" );
        block_FifoEmpty( p_owner->p_fifo );
    }
    block_FifoPut( p_owner->p_fifo, p_block );
}
