 */
void vlc_event_manager_fini( vlc_event_manager_t * p_em )
{
    struct vlc_event_listeners_group_t * listeners_group;
    struct vlc_event_listener_t * listener;
    vlc_mutex_destroy( &p_em->object_lock );
    vlc_mutex_destroy( &p_em->event_sending_lock );
    FOREACH_ARRAY( listeners_group, p_em->listeners_groups )
        FOREACH_ARRAY( listener, listeners_group->listeners )
            free( listener );
        FOREACH_END()
        ARRAY_RESET( listeners_group->listeners );
        free( listeners_group );
    FOREACH_END()
    ARRAY_RESET( p_em->listeners_groups );
}
