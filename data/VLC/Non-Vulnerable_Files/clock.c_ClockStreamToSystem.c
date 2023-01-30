 *****************************************************************************/
static mtime_t ClockStreamToSystem( input_clock_t *cl, mtime_t i_stream )
{
    if( !cl->b_has_reference )
        return VLC_TS_INVALID;
    return ( i_stream - cl->ref.i_stream ) * cl->i_rate / INPUT_RATE_DEFAULT +
           cl->ref.i_system;
}
