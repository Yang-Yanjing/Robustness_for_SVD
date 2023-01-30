 */
static mtime_t ClockGetTsOffset( input_clock_t *cl )
{
    return cl->i_pts_delay * ( cl->i_rate - INPUT_RATE_DEFAULT ) / INPUT_RATE_DEFAULT;
}
