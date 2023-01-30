}
void input_DecoderGetObjects( decoder_t *p_dec,
                              vout_thread_t **pp_vout, audio_output_t **pp_aout )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    if( pp_vout )
        *pp_vout = p_owner->p_vout ? vlc_object_hold( p_owner->p_vout ) : NULL;
    if( pp_aout )
        *pp_aout = p_owner->p_aout ? vlc_object_hold( p_owner->p_aout ) : NULL;
    vlc_mutex_unlock( &p_owner->lock );
}
