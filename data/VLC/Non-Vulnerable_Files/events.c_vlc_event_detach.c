 */
void vlc_event_detach( vlc_event_manager_t *p_em,
                       vlc_event_type_t event_type,
                       vlc_event_callback_t pf_callback,
                       void *p_user_data )
{
    vlc_event_listeners_group_t * listeners_group;
    struct vlc_event_listener_t * listener;
    vlc_mutex_lock( &p_em->event_sending_lock );
    vlc_mutex_lock( &p_em->object_lock );
    FOREACH_ARRAY( listeners_group, p_em->listeners_groups )
        if( listeners_group->event_type == event_type )
        {
            FOREACH_ARRAY( listener, listeners_group->listeners )
                if( listener->pf_callback == pf_callback &&
                    listener->p_user_data == p_user_data )
                {
                    /* Tell vlc_event_send, we did remove an item from that group,
                       in case vlc_event_send is in our caller stack  */
                    listeners_group->b_sublistener_removed = true;
                    /* that's our listener */
                    ARRAY_REMOVE( listeners_group->listeners,
                        fe_idx /* This comes from the macro (and that's why
                                  I hate macro) */ );
                    free( listener );
                    vlc_mutex_unlock( &p_em->event_sending_lock );
                    vlc_mutex_unlock( &p_em->object_lock );
                    return;
                }
            FOREACH_END()
        }
    FOREACH_END()
    assert( 0 );
}
