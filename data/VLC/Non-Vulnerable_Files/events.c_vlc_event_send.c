 */
void vlc_event_send( vlc_event_manager_t * p_em,
                     vlc_event_t * p_event )
{
    vlc_event_listeners_group_t * listeners_group = NULL;
    vlc_event_listener_t * listener;
    vlc_event_listener_t * array_of_cached_listeners = NULL;
    vlc_event_listener_t * cached_listener;
    int i, i_cached_listeners = 0;
    /* Fill event with the sending object now */
    p_event->p_obj = p_em->p_obj;
    vlc_mutex_lock( &p_em->event_sending_lock ) ;
    vlc_mutex_lock( &p_em->object_lock );
    FOREACH_ARRAY( listeners_group, p_em->listeners_groups )
        if( listeners_group->event_type == p_event->type )
        {
            if( listeners_group->listeners.i_size <= 0 )
                break;
            /* Save the function to call */
            i_cached_listeners = listeners_group->listeners.i_size;
            array_of_cached_listeners = malloc(
                    sizeof(vlc_event_listener_t)*i_cached_listeners );
            if( !array_of_cached_listeners )
            {
                vlc_mutex_unlock( &p_em->object_lock );
                vlc_mutex_unlock( &p_em->event_sending_lock ) ;
                return;
            }
            cached_listener = array_of_cached_listeners;
            FOREACH_ARRAY( listener, listeners_group->listeners )
                memcpy( cached_listener, listener, sizeof(vlc_event_listener_t));
                cached_listener++;
            FOREACH_END()
            break;
        }
    FOREACH_END()
    /* Track item removed from *this* thread, with a simple flag. Indeed
     * event_sending_lock is a recursive lock. This has the advantage of
     * allowing to remove an event listener from within a callback */
    listeners_group->b_sublistener_removed = false;
    vlc_mutex_unlock( &p_em->object_lock );
    /* Call the function attached */
    cached_listener = array_of_cached_listeners;
    if( !listeners_group || !array_of_cached_listeners )
    {
        free( array_of_cached_listeners );
        vlc_mutex_unlock( &p_em->event_sending_lock );
        return;
    }
    for( i = 0; i < i_cached_listeners; i++ )
    {
        if( listeners_group->b_sublistener_removed )
        {
            /* If a callback was removed inside one of our callback, this gets
	     * called */
            bool valid_listener;
            vlc_mutex_lock( &p_em->object_lock );
            valid_listener = group_contains_listener( listeners_group, cached_listener );
            vlc_mutex_unlock( &p_em->object_lock );
            if( !valid_listener )
            {
                cached_listener++;
                continue;
            }
        }
        cached_listener->pf_callback( p_event, cached_listener->p_user_data );
        cached_listener++;
    }
    vlc_mutex_unlock( &p_em->event_sending_lock );
    free( array_of_cached_listeners );
}
