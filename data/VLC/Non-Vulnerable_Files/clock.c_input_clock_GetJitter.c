}
mtime_t input_clock_GetJitter( input_clock_t *cl )
{
    vlc_mutex_lock( &cl->lock );
#if INPUT_CLOCK_LATE_COUNT != 3
#   error "unsupported INPUT_CLOCK_LATE_COUNT"
#endif
    /* Find the median of the last late values
     * It works pretty well at rejecting bad values
     *
     * XXX we only increase pts_delay over time, decreasing it is
     * not that easy if we want to be robust.
     */
    const mtime_t *p = cl->late.pi_value;
    mtime_t i_late_median = p[0] + p[1] + p[2] - __MIN(__MIN(p[0],p[1]),p[2]) - __MAX(__MAX(p[0],p[1]),p[2]);
    mtime_t i_pts_delay = cl->i_pts_delay ;
    vlc_mutex_unlock( &cl->lock );
    return i_pts_delay + i_late_median;
}
