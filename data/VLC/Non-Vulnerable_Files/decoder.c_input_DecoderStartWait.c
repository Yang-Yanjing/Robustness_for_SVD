}
void input_DecoderStartWait( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    DecoderFlush( p_dec );
    p_owner->b_first = true;
    p_owner->b_has_data = false;
    p_owner->b_waiting = true;
    vlc_cond_signal( &p_owner->wait_request );
    vlc_mutex_unlock( &p_owner->lock );
}
