}
static void DecoderFixTs( decoder_t *p_dec, mtime_t *pi_ts0, mtime_t *pi_ts1,
                          mtime_t *pi_duration, int *pi_rate, mtime_t i_ts_bound )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    input_clock_t   *p_clock = p_owner->p_clock;
    vlc_assert_locked( &p_owner->lock );
    const mtime_t i_es_delay = p_owner->i_ts_delay;
    if( !p_clock )
        return;
    const bool b_ephemere = pi_ts1 && *pi_ts0 == *pi_ts1;
    int i_rate;
    if( *pi_ts0 > VLC_TS_INVALID )
    {
        *pi_ts0 += i_es_delay;
        if( pi_ts1 && *pi_ts1 > VLC_TS_INVALID )
            *pi_ts1 += i_es_delay;
        if( input_clock_ConvertTS( VLC_OBJECT(p_dec), p_clock, &i_rate, pi_ts0, pi_ts1, i_ts_bound ) ) {
            if( pi_ts1 != NULL )
                msg_Err(p_dec, "Could not convert timestamps %"PRId64
                        ", %"PRId64"", *pi_ts0, *pi_ts1);
            else
                msg_Err(p_dec, "Could not convert timestamp %"PRId64, *pi_ts0);
            *pi_ts0 = VLC_TS_INVALID;
        }
    }
    else
    {
        i_rate = input_clock_GetRate( p_clock );
    }
    /* Do not create ephemere data because of rounding errors */
    if( !b_ephemere && pi_ts1 && *pi_ts0 == *pi_ts1 )
        *pi_ts1 += 1;
    if( pi_duration )
        *pi_duration = ( *pi_duration * i_rate + INPUT_RATE_DEFAULT-1 )
            / INPUT_RATE_DEFAULT;
    if( pi_rate )
        *pi_rate = i_rate;
}
