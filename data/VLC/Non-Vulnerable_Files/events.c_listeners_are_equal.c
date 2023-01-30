static bool
listeners_are_equal( vlc_event_listener_t * listener1,
                     vlc_event_listener_t * listener2 )
{
    return listener1->pf_callback == listener2->pf_callback &&
           listener1->p_user_data == listener2->p_user_data;
}
