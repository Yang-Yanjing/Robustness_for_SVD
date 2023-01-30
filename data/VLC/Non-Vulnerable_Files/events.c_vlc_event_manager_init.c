 */
int vlc_event_manager_init( vlc_event_manager_t * p_em, void * p_obj )
{
    p_em->p_obj = p_obj;
    vlc_mutex_init( &p_em->object_lock );
    /* We need a recursive lock here, because we need to be able
     * to call libvlc_event_detach even if vlc_event_send is in
     * the call frame.
     * This ensures that after libvlc_event_detach, the callback
     * will never gets triggered.
     * */
    vlc_mutex_init_recursive( &p_em->event_sending_lock );
    ARRAY_INIT( p_em->listeners_groups );
    return VLC_SUCCESS;
}
