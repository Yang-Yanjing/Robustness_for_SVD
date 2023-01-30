#warning "input_clock_SetJitter needs more work"
void input_clock_SetJitter( input_clock_t *cl,
                            mtime_t i_pts_delay, int i_cr_average )
{
    vlc_mutex_lock( &cl->lock );
    /* Update late observations */
    const mtime_t i_delay_delta = i_pts_delay - cl->i_pts_delay;
    mtime_t pi_late[INPUT_CLOCK_LATE_COUNT];
    for( int i = 0; i < INPUT_CLOCK_LATE_COUNT; i++ )
        pi_late[i] = __MAX( cl->late.pi_value[(cl->late.i_index + 1 + i)%INPUT_CLOCK_LATE_COUNT] - i_delay_delta, 0 );
    for( int i = 0; i < INPUT_CLOCK_LATE_COUNT; i++ )
        cl->late.pi_value[i] = 0;
    cl->late.i_index = 0;
    for( int i = 0; i < INPUT_CLOCK_LATE_COUNT; i++ )
    {
        if( pi_late[i] <= 0 )
            continue;
        cl->late.pi_value[cl->late.i_index] = pi_late[i];
        cl->late.i_index = ( cl->late.i_index + 1 ) % INPUT_CLOCK_LATE_COUNT;
    }
    /* TODO always save the value, and when rebuffering use the new one if smaller
     * TODO when increasing -> force rebuffering
     */
    if( cl->i_pts_delay < i_pts_delay )
        cl->i_pts_delay = i_pts_delay;
    /* */
    if( i_cr_average < 10 )
        i_cr_average = 10;
    if( cl->drift.i_divider != i_cr_average )
        AvgRescale( &cl->drift, i_cr_average );
    vlc_mutex_unlock( &cl->lock );
}
