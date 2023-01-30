 *****************************************************************************/
input_clock_t *input_clock_New( int i_rate )
{
    input_clock_t *cl = malloc( sizeof(*cl) );
    if( !cl )
        return NULL;
    vlc_mutex_init( &cl->lock );
    cl->b_has_reference = false;
    cl->ref = clock_point_Create( VLC_TS_INVALID, VLC_TS_INVALID );
    cl->b_has_external_clock = false;
    cl->last = clock_point_Create( VLC_TS_INVALID, VLC_TS_INVALID );
    cl->i_ts_max = VLC_TS_INVALID;
    cl->i_buffering_duration = 0;
    cl->i_next_drift_update = VLC_TS_INVALID;
    AvgInit( &cl->drift, 10 );
    cl->late.i_index = 0;
    for( int i = 0; i < INPUT_CLOCK_LATE_COUNT; i++ )
        cl->late.pi_value[i] = 0;
    cl->i_rate = i_rate;
    cl->i_pts_delay = 0;
    cl->b_paused = false;
    cl->i_pause_date = VLC_TS_INVALID;
    return cl;
}
