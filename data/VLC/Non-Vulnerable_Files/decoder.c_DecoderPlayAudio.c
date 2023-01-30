}
static void DecoderPlayAudio( decoder_t *p_dec, block_t *p_audio,
                              int *pi_played_sum, int *pi_lost_sum )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    audio_output_t *p_aout = p_owner->p_aout;
    /* */
    if( p_audio && p_audio->i_pts <= VLC_TS_INVALID ) // FIXME --VLC_TS_INVALID verify audio_output/*
    {
        msg_Warn( p_dec, "non-dated audio buffer received" );
        *pi_lost_sum += 1;
        block_Release( p_audio );
        return;
    }
    /* */
    vlc_mutex_lock( &p_owner->lock );
    if( p_audio && p_owner->b_waiting )
    {
        p_owner->b_has_data = true;
        vlc_cond_signal( &p_owner->wait_acknowledge );
    }
    for( ;; )
    {
        bool b_paused;
        bool b_reject = DecoderWaitUnblock( p_dec );
        b_paused = p_owner->b_paused;
        if (!p_audio)
            break;
        /* */
        int i_rate = INPUT_RATE_DEFAULT;
        DecoderFixTs( p_dec, &p_audio->i_pts, NULL, &p_audio->i_length,
                      &i_rate, AOUT_MAX_ADVANCE_TIME );
        if( p_audio->i_pts <= VLC_TS_INVALID
         || i_rate < INPUT_RATE_DEFAULT/AOUT_MAX_INPUT_RATE
         || i_rate > INPUT_RATE_DEFAULT*AOUT_MAX_INPUT_RATE )
            b_reject = true;
        DecoderWaitDate( p_dec, &b_reject,
                         p_audio->i_pts - AOUT_MAX_PREPARE_TIME );
        if( unlikely(p_owner->b_paused != b_paused) )
            continue; /* race with input thread? retry... */
        if( p_aout == NULL )
            b_reject = true;
        if( !b_reject )
        {
            assert( !p_owner->b_paused );
            if( !aout_DecPlay( p_aout, p_audio, i_rate ) )
                *pi_played_sum += 1;
            *pi_lost_sum += aout_DecGetResetLost( p_aout );
        }
        else
        {
            msg_Dbg( p_dec, "discarded audio buffer" );
            *pi_lost_sum += 1;
            block_Release( p_audio );
        }
        break;
    }
    vlc_mutex_unlock( &p_owner->lock );
}
