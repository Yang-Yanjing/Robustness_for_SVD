}
void input_clock_GetSystemOrigin( input_clock_t *cl, mtime_t *pi_system, mtime_t *pi_delay )
{
    vlc_mutex_lock( &cl->lock );
    assert( cl->b_has_reference );
    *pi_system = cl->ref.i_system;
    if( pi_delay )
        *pi_delay  = cl->i_pts_delay;
    vlc_mutex_unlock( &cl->lock );
}
