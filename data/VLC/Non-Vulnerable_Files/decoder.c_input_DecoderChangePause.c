}
void input_DecoderChangePause( decoder_t *p_dec, bool b_paused, mtime_t i_date )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    /* Normally, p_owner->b_paused != b_paused here. But if a track is added
     * while the input is paused (e.g. add sub file), then b_paused is
     * (incorrectly) false. */
    if( likely(p_owner->b_paused != b_paused) ) {
        p_owner->b_paused = b_paused;
        p_owner->pause.i_date = i_date;
        p_owner->pause.i_ignore = 0;
        vlc_cond_signal( &p_owner->wait_request );
        DecoderOutputChangePause( p_dec, b_paused, i_date );
    }
    vlc_mutex_unlock( &p_owner->lock );
}
