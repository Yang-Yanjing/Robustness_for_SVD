 *****************************************************************************/
void input_clock_Update( input_clock_t *cl, vlc_object_t *p_log,
                         bool *pb_late,
                         bool b_can_pace_control, bool b_buffering_allowed,
                         mtime_t i_ck_stream, mtime_t i_ck_system )
{
    bool b_reset_reference = false;
    assert( i_ck_stream > VLC_TS_INVALID && i_ck_system > VLC_TS_INVALID );
    vlc_mutex_lock( &cl->lock );
    if( !cl->b_has_reference )
    {
        /* */
        b_reset_reference= true;
    }
    else if( cl->last.i_stream > VLC_TS_INVALID &&
             ( (cl->last.i_stream - i_ck_stream) > CR_MAX_GAP ||
               (cl->last.i_stream - i_ck_stream) < -CR_MAX_GAP ) )
    {
        /* Stream discontinuity, for which we haven't received a
         * warning from the stream control facilities (dd-edited
         * stream ?). */
        msg_Warn( p_log, "clock gap, unexpected stream discontinuity" );
        cl->i_ts_max = VLC_TS_INVALID;
        /* */
        msg_Warn( p_log, "feeding synchro with a new reference point trying to recover from clock gap" );
        b_reset_reference= true;
    }
    /* */
    if( b_reset_reference )
    {
        cl->i_next_drift_update = VLC_TS_INVALID;
        AvgReset( &cl->drift );
        /* Feed synchro with a new reference point. */
        cl->b_has_reference = true;
        cl->ref = clock_point_Create( i_ck_stream,
                                      __MAX( cl->i_ts_max + CR_MEAN_PTS_GAP, i_ck_system ) );
        cl->b_has_external_clock = false;
    }
    /* Compute the drift between the stream clock and the system clock
     * when we don't control the source pace */
    if( !b_can_pace_control && cl->i_next_drift_update < i_ck_system )
    {
        const mtime_t i_converted = ClockSystemToStream( cl, i_ck_system );
        AvgUpdate( &cl->drift, i_converted - i_ck_stream );
        cl->i_next_drift_update = i_ck_system + CLOCK_FREQ/5; /* FIXME why that */
    }
    /* Update the extra buffering value */
    if( !b_can_pace_control || b_reset_reference )
    {
        cl->i_buffering_duration = 0;
    }
    else if( b_buffering_allowed )
    {
        /* Try to bufferize more than necessary by reading
         * CR_BUFFERING_RATE/256 faster until we have CR_BUFFERING_TARGET.
         */
        const mtime_t i_duration = __MAX( i_ck_stream - cl->last.i_stream, 0 );
        cl->i_buffering_duration += ( i_duration * CR_BUFFERING_RATE + 255 ) / 256;
        if( cl->i_buffering_duration > CR_BUFFERING_TARGET )
            cl->i_buffering_duration = CR_BUFFERING_TARGET;
    }
    //fprintf( stderr, "input_clock_Update: %d :: %lld\n", b_buffering_allowed, cl->i_buffering_duration/1000 );
    /* */
    cl->last = clock_point_Create( i_ck_stream, i_ck_system );
    /* It does not take the decoder latency into account but it is not really
     * the goal of the clock here */
    const mtime_t i_system_expected = ClockStreamToSystem( cl, i_ck_stream + AvgGet( &cl->drift ) );
    const mtime_t i_late = ( i_ck_system - cl->i_pts_delay ) - i_system_expected;
    *pb_late = i_late > 0;
    if( i_late > 0 )
    {
        cl->late.pi_value[cl->late.i_index] = i_late;
        cl->late.i_index = ( cl->late.i_index + 1 ) % INPUT_CLOCK_LATE_COUNT;
    }
    vlc_mutex_unlock( &cl->lock );
}
