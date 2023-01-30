}
void input_DecoderFrameNext( decoder_t *p_dec, mtime_t *pi_duration )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    *pi_duration = 0;
    vlc_mutex_lock( &p_owner->lock );
    if( p_dec->fmt_out.i_cat == VIDEO_ES )
    {
        if( p_owner->b_paused && p_owner->p_vout )
        {
            vout_NextPicture( p_owner->p_vout, pi_duration );
            p_owner->pause.i_ignore++;
            vlc_cond_signal( &p_owner->wait_request );
        }
    }
    else
    {
        /* TODO subtitle should not be flushed */
        DecoderFlush( p_dec );
    }
    vlc_mutex_unlock( &p_owner->lock );
}
