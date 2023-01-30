 *****************************************************************************/
void input_clock_ChangePause( input_clock_t *cl, bool b_paused, mtime_t i_date )
{
    vlc_mutex_lock( &cl->lock );
    assert( (!cl->b_paused) != (!b_paused) );
    if( cl->b_paused )
    {
        const mtime_t i_duration = i_date - cl->i_pause_date;
        if( cl->b_has_reference && i_duration > 0 )
        {
            cl->ref.i_system += i_duration;
            cl->last.i_system += i_duration;
        }
    }
    cl->i_pause_date = i_date;
    cl->b_paused = b_paused;
    vlc_mutex_unlock( &cl->lock );
}
