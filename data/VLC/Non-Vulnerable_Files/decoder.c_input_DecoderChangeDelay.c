}
void input_DecoderChangeDelay( decoder_t *p_dec, mtime_t i_delay )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    p_owner->i_ts_delay = i_delay;
    vlc_mutex_unlock( &p_owner->lock );
}
