static bool
group_contains_listener( vlc_event_listeners_group_t * group,
                         vlc_event_listener_t * searched_listener )
{
    vlc_event_listener_t * listener;
    FOREACH_ARRAY( listener, group->listeners )
        if( listeners_are_equal(searched_listener, listener) )
            return true;
    FOREACH_END()
    return false;
}
