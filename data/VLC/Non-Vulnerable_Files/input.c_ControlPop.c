}
static inline int ControlPop( input_thread_t *p_input,
                              int *pi_type, vlc_value_t *p_val,
                              mtime_t i_deadline, bool b_postpone_seek )
{
    input_thread_private_t *p_sys = p_input->p;
    vlc_mutex_lock( &p_sys->lock_control );
    while( p_sys->i_control <= 0 ||
           ( b_postpone_seek && ControlIsSeekRequest( p_sys->control[0].i_type ) ) )
    {
        if( !vlc_object_alive( p_input ) || i_deadline < 0 )
        {
            vlc_mutex_unlock( &p_sys->lock_control );
            return VLC_EGENERIC;
        }
        if( vlc_cond_timedwait( &p_sys->wait_control, &p_sys->lock_control,
                                i_deadline ) )
        {
            vlc_mutex_unlock( &p_sys->lock_control );
            return VLC_EGENERIC;
        }
    }
    /* */
    const int i_index = ControlGetReducedIndexLocked( p_input );
    /* */
    *pi_type = p_sys->control[i_index].i_type;
    *p_val   = p_sys->control[i_index].val;
    p_sys->i_control -= i_index + 1;
    if( p_sys->i_control > 0 )
        memmove( &p_sys->control[0], &p_sys->control[i_index+1],
                 sizeof(*p_sys->control) * p_sys->i_control );
    vlc_mutex_unlock( &p_sys->lock_control );
    return VLC_SUCCESS;
}
