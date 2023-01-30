 */
void input_DecoderDelete( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_cancel( p_owner->thread );
    /* Make sure we aren't paused/waiting/decoding anymore */
    vlc_mutex_lock( &p_owner->lock );
    const bool b_was_paused = p_owner->b_paused;
    p_owner->b_paused = false;
    p_owner->b_waiting = false;
    p_owner->b_flushing = true;
    p_owner->b_exit = true;
    vlc_cond_signal( &p_owner->wait_request );
    vlc_mutex_unlock( &p_owner->lock );
    vlc_join( p_owner->thread, NULL );
    p_owner->b_paused = b_was_paused;
    module_unneed( p_dec, p_dec->p_module );
    /* */
    if( p_dec->p_owner->cc.b_supported )
    {
        int i;
        for( i = 0; i < 4; i++ )
            input_DecoderSetCcState( p_dec, false, i );
    }
    /* Delete decoder */
    DeleteDecoder( p_dec );
}
