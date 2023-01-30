}
int input_clock_GetState( input_clock_t *cl,
                          mtime_t *pi_stream_start, mtime_t *pi_system_start,
                          mtime_t *pi_stream_duration, mtime_t *pi_system_duration )
{
    vlc_mutex_lock( &cl->lock );
    if( !cl->b_has_reference )
    {
        vlc_mutex_unlock( &cl->lock );
        return VLC_EGENERIC;
    }
    *pi_stream_start = cl->ref.i_stream;
    *pi_system_start = cl->ref.i_system;
    *pi_stream_duration = cl->last.i_stream - cl->ref.i_stream;
    *pi_system_duration = cl->last.i_system - cl->ref.i_system;
    vlc_mutex_unlock( &cl->lock );
    return VLC_SUCCESS;
}
}
int input_clock_GetState( input_clock_t *cl,
                          mtime_t *pi_stream_start, mtime_t *pi_system_start,
                          mtime_t *pi_stream_duration, mtime_t *pi_system_duration )
{
    vlc_mutex_lock( &cl->lock );
    if( !cl->b_has_reference )
    {
        vlc_mutex_unlock( &cl->lock );
        return VLC_EGENERIC;
    }
