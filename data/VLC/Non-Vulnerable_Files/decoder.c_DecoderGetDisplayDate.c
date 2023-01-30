}
static mtime_t DecoderGetDisplayDate( decoder_t *p_dec, mtime_t i_ts )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_mutex_lock( &p_owner->lock );
    if( p_owner->b_waiting || p_owner->b_paused )
        i_ts = VLC_TS_INVALID;
    vlc_mutex_unlock( &p_owner->lock );
    if( !p_owner->p_clock || i_ts <= VLC_TS_INVALID )
        return i_ts;
    if( input_clock_ConvertTS( VLC_OBJECT(p_dec), p_owner->p_clock, NULL, &i_ts, NULL, INT64_MAX ) ) {
        msg_Err(p_dec, "Could not get display date for timestamp %"PRId64"", i_ts);
        return VLC_TS_INVALID;
    }
    return i_ts;
}
