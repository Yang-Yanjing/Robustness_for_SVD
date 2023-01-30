}
void input_DecoderWait( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    while( p_owner->b_waiting && !p_owner->b_has_data )
    {
        block_FifoWake( p_owner->p_fifo );
        vlc_cond_wait( &p_owner->wait_acknowledge, &p_owner->lock );
    }
    vlc_mutex_unlock( &p_owner->lock );
}
