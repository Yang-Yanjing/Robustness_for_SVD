}
void input_DecoderStopWait( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    p_owner->b_waiting = false;
    vlc_cond_signal( &p_owner->wait_request );
    vlc_mutex_unlock( &p_owner->lock );
}
