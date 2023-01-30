}
static void DecoderPlaySpu( decoder_t *p_dec, subpicture_t *p_subpic )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vout_thread_t *p_vout = p_owner->p_spu_vout;
    /* */
    if( p_subpic->i_start <= VLC_TS_INVALID )
    {
        msg_Warn( p_dec, "non-dated spu buffer received" );
        subpicture_Delete( p_subpic );
        return;
    }
    /* */
    vlc_mutex_lock( &p_owner->lock );
    if( p_owner->b_waiting )
    {
        p_owner->b_has_data = true;
        vlc_cond_signal( &p_owner->wait_acknowledge );
    }
    bool b_reject = DecoderWaitUnblock( p_dec );
    DecoderFixTs( p_dec, &p_subpic->i_start, &p_subpic->i_stop, NULL,
                  NULL, INT64_MAX );
    if( p_subpic->i_start <= VLC_TS_INVALID )
        b_reject = true;
    DecoderWaitDate( p_dec, &b_reject,
                     p_subpic->i_start - SPU_MAX_PREPARE_TIME );
    vlc_mutex_unlock( &p_owner->lock );
    if( !b_reject )
        vout_PutSubpicture( p_vout, p_subpic );
    else
        subpicture_Delete( p_subpic );
}
