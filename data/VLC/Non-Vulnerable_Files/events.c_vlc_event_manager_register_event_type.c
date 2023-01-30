 */
int vlc_event_manager_register_event_type(
        vlc_event_manager_t * p_em,
        vlc_event_type_t event_type )
{
    vlc_event_listeners_group_t * listeners_group;
    listeners_group = malloc(sizeof(vlc_event_listeners_group_t));
    if( !listeners_group )
        return VLC_ENOMEM;
    listeners_group->event_type = event_type;
    ARRAY_INIT( listeners_group->listeners );
    vlc_mutex_lock( &p_em->object_lock );
    ARRAY_APPEND( p_em->listeners_groups, listeners_group );
    vlc_mutex_unlock( &p_em->object_lock );
    return VLC_SUCCESS;
}
