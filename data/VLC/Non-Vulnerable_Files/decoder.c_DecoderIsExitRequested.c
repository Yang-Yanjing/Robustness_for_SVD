}
static bool DecoderIsExitRequested( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    bool b_exit = p_owner->b_exit;
    vlc_mutex_unlock( &p_owner->lock );
    return b_exit;
}
