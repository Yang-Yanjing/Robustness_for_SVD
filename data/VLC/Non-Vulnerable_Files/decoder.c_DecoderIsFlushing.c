}
static bool DecoderIsFlushing( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    bool b_flushing;
    vlc_mutex_lock( &p_owner->lock );
    b_flushing = p_owner->b_flushing;
    vlc_mutex_unlock( &p_owner->lock );
    return b_flushing;
}
