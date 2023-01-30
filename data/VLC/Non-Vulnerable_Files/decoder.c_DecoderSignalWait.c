}
static void DecoderSignalWait( decoder_t *p_dec, bool b_has_data )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    if( p_owner->b_waiting )
    {
        if( b_has_data )
            p_owner->b_has_data = true;
        vlc_cond_signal( &p_owner->wait_acknowledge );
    }
    vlc_mutex_unlock( &p_owner->lock );
}
