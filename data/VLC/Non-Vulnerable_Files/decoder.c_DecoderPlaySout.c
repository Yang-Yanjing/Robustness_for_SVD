#ifdef ENABLE_SOUT
static int DecoderPlaySout( decoder_t *p_dec, block_t *p_sout_block )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    assert( p_owner->p_clock );
    assert( !p_sout_block->p_next );
    vlc_mutex_lock( &p_owner->lock );
    if( p_owner->b_waiting )
    {
        p_owner->b_has_data = true;
        vlc_cond_signal( &p_owner->wait_acknowledge );
    }
    bool b_reject = DecoderWaitUnblock( p_dec );
    DecoderFixTs( p_dec, &p_sout_block->i_dts, &p_sout_block->i_pts,
                  &p_sout_block->i_length, NULL, INT64_MAX );
    vlc_mutex_unlock( &p_owner->lock );
    if( !b_reject )
    {
        /* FIXME --VLC_TS_INVALID inspect stream_output*/
        return sout_InputSendBuffer( p_owner->p_sout_input, p_sout_block );
    }
    else
    {
        block_Release( p_sout_block );
        return VLC_EGENERIC;
    }
}
