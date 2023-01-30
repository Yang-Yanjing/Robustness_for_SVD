}
int input_DecoderGetCcState( decoder_t *p_dec, bool *pb_decode, int i_channel )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    *pb_decode = false;
    if( i_channel < 0 || i_channel >= 4 || !p_owner->cc.pb_present[i_channel] )
        return VLC_EGENERIC;
    vlc_mutex_lock( &p_owner->lock );
    *pb_decode = p_owner->cc.pp_decoder[i_channel] != NULL;
    vlc_mutex_unlock( &p_owner->lock );
    return VLC_EGENERIC;
}
