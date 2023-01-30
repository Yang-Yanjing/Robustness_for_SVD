 */
int vlc_event_attach( vlc_event_manager_t * p_em,
                      vlc_event_type_t event_type,
                      vlc_event_callback_t pf_callback,
                      void *p_user_data )
{
    vlc_event_listeners_group_t * listeners_group;
    vlc_event_listener_t * listener;
    listener = malloc(sizeof(vlc_event_listener_t));
    if( !listener )
        return VLC_ENOMEM;
    listener->p_user_data = p_user_data;
    listener->pf_callback = pf_callback;
    vlc_mutex_lock( &p_em->object_lock );
    FOREACH_ARRAY( listeners_group, p_em->listeners_groups )
        if( listeners_group->event_type == event_type )
        {
            ARRAY_APPEND( listeners_group->listeners, listener );
            vlc_mutex_unlock( &p_em->object_lock );
            return VLC_SUCCESS;
        }
    FOREACH_END()
    /* Unknown event = BUG */
    assert( 0 );
}
