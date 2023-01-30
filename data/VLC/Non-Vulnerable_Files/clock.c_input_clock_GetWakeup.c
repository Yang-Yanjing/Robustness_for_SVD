 *****************************************************************************/
mtime_t input_clock_GetWakeup( input_clock_t *cl )
{
    mtime_t i_wakeup = 0;
    vlc_mutex_lock( &cl->lock );
    /* Synchronized, we can wait */
    if( cl->b_has_reference )
        i_wakeup = ClockStreamToSystem( cl, cl->last.i_stream + AvgGet( &cl->drift ) - cl->i_buffering_duration );
    vlc_mutex_unlock( &cl->lock );
    return i_wakeup;
}
