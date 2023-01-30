 *****************************************************************************/
static mtime_t ClockSystemToStream( input_clock_t *cl, mtime_t i_system )
{
    assert( cl->b_has_reference );
    return ( i_system - cl->ref.i_system ) * INPUT_RATE_DEFAULT / cl->i_rate +
            cl->ref.i_stream;
}
