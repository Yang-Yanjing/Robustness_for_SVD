}
static void DecoderPlayVideo( decoder_t *p_dec, picture_t *p_picture,
                              int *pi_played_sum, int *pi_lost_sum )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vout_thread_t  *p_vout = p_owner->p_vout;
    if( p_picture->date <= VLC_TS_INVALID )
    {
        msg_Warn( p_dec, "non-dated video buffer received" );
        *pi_lost_sum += 1;
        vout_ReleasePicture( p_vout, p_picture );
        return;
    }
    /* */
    vlc_mutex_lock( &p_owner->lock );
    if( p_owner->b_waiting && !p_owner->b_first )
    {
        p_owner->b_has_data = true;
        vlc_cond_signal( &p_owner->wait_acknowledge );
    }
    bool b_first_after_wait = p_owner->b_waiting && p_owner->b_has_data;
    bool b_reject = DecoderWaitUnblock( p_dec );
    if( p_owner->b_waiting )
    {
        assert( p_owner->b_first );
        msg_Dbg( p_dec, "Received first picture" );
        p_owner->b_first = false;
        p_picture->b_force = true;
    }
    const bool b_dated = p_picture->date > VLC_TS_INVALID;
    int i_rate = INPUT_RATE_DEFAULT;
    DecoderFixTs( p_dec, &p_picture->date, NULL, NULL,
                  &i_rate, DECODER_BOGUS_VIDEO_DELAY );
    vlc_mutex_unlock( &p_owner->lock );
    /* */
    if( !p_picture->b_force && p_picture->date <= VLC_TS_INVALID ) // FIXME --VLC_TS_INVALID verify video_output/*
        b_reject = true;
    if( !b_reject )
    {
        if( i_rate != p_owner->i_last_rate || b_first_after_wait )
        {
            /* Be sure to not display old picture after our own */
            vout_Flush( p_vout, p_picture->date );
            p_owner->i_last_rate = i_rate;
        }
        vout_PutPicture( p_vout, p_picture );
    }
    else
    {
        if( b_dated )
            msg_Warn( p_dec, "early picture skipped" );
        else
            msg_Warn( p_dec, "non-dated video buffer received" );
        *pi_lost_sum += 1;
        vout_ReleasePicture( p_vout, p_picture );
    }
    int i_tmp_display;
    int i_tmp_lost;
    vout_GetResetStatistic( p_vout, &i_tmp_display, &i_tmp_lost );
    *pi_played_sum += i_tmp_display;
    *pi_lost_sum += i_tmp_lost;
}
