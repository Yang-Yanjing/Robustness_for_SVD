}
static void DecoderOutputChangePause( decoder_t *p_dec, bool b_paused, mtime_t i_date )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_assert_locked( &p_owner->lock );
    /* XXX only audio and video output have to be paused.
     * - for sout it is useless
     * - for subs, it is done by the vout
     */
    if( p_dec->fmt_out.i_cat == AUDIO_ES )
    {
        if( p_owner->p_aout )
            aout_DecChangePause( p_owner->p_aout, b_paused, i_date );
    }
    else if( p_dec->fmt_out.i_cat == VIDEO_ES )
    {
        if( p_owner->p_vout )
            vout_ChangePause( p_owner->p_vout, b_paused, i_date );
    }
}
