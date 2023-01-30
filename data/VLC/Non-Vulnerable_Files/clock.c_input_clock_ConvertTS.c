 *****************************************************************************/
int input_clock_ConvertTS( vlc_object_t *p_object, input_clock_t *cl,
                           int *pi_rate, mtime_t *pi_ts0, mtime_t *pi_ts1,
                           mtime_t i_ts_bound )
{
    assert( pi_ts0 );
    vlc_mutex_lock( &cl->lock );
    if( pi_rate )
        *pi_rate = cl->i_rate;
    if( !cl->b_has_reference )
    {
        vlc_mutex_unlock( &cl->lock );
        *pi_ts0 = VLC_TS_INVALID;
        if( pi_ts1 )
            *pi_ts1 = VLC_TS_INVALID;
        return VLC_EGENERIC;
    }
    /* */
    const mtime_t i_ts_buffering = cl->i_buffering_duration * cl->i_rate / INPUT_RATE_DEFAULT;
    const mtime_t i_ts_delay = cl->i_pts_delay + ClockGetTsOffset( cl );
    /* */
    if( *pi_ts0 > VLC_TS_INVALID )
    {
        *pi_ts0 = ClockStreamToSystem( cl, *pi_ts0 + AvgGet( &cl->drift ) );
        if( *pi_ts0 > cl->i_ts_max )
            cl->i_ts_max = *pi_ts0;
        *pi_ts0 += i_ts_delay;
    }
    /* XXX we do not update i_ts_max on purpose */
    if( pi_ts1 && *pi_ts1 > VLC_TS_INVALID )
    {
        *pi_ts1 = ClockStreamToSystem( cl, *pi_ts1 + AvgGet( &cl->drift ) ) +
                  i_ts_delay;
    }
    vlc_mutex_unlock( &cl->lock );
    /* Check ts validity */
    if (i_ts_bound != INT64_MAX && *pi_ts0 > VLC_TS_INVALID) {
        if (*pi_ts0 >= mdate() + i_ts_delay + i_ts_buffering + i_ts_bound) {
            vlc_Log(p_object, VLC_MSG_ERR, "clock",
                "Timestamp conversion failed (delay %"PRId64", buffering "
                "%"PRId64", bound %"PRId64")",
                i_ts_delay, i_ts_buffering, i_ts_bound);
            return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
