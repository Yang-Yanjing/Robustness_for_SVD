}
void input_DecoderIsCcPresent( decoder_t *p_dec, bool pb_present[4] )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    int i;
    vlc_mutex_lock( &p_owner->lock );
    for( i = 0; i < 4; i++ )
        pb_present[i] =  p_owner->cc.pb_present[i];
    vlc_mutex_unlock( &p_owner->lock );
}
