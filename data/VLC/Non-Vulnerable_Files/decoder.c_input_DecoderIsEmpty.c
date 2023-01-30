}
bool input_DecoderIsEmpty( decoder_t * p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    assert( !p_owner->b_waiting );
    bool b_empty = block_FifoCount( p_dec->p_owner->p_fifo ) <= 0;
    if( b_empty )
    {
        vlc_mutex_lock( &p_owner->lock );
        /* TODO subtitles support */
        if( p_dec->fmt_out.i_cat == VIDEO_ES && p_owner->p_vout )
            b_empty = vout_IsEmpty( p_owner->p_vout );
        else if( p_dec->fmt_out.i_cat == AUDIO_ES && p_owner->p_aout )
            b_empty = aout_DecIsEmpty( p_owner->p_aout );
        vlc_mutex_unlock( &p_owner->lock );
    }
    return b_empty;
}
