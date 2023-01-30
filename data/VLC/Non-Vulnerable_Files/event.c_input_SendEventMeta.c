 */
void input_SendEventMeta( input_thread_t *p_input )
{
    Trigger( p_input, INPUT_EVENT_ITEM_META );
    /* FIXME remove this ugliness ? */
    vlc_event_t event;
    event.type = vlc_InputItemMetaChanged;
    event.u.input_item_meta_changed.meta_type = vlc_meta_ArtworkURL;
    vlc_event_send( &p_input->p->p_item->event_manager, &event );
}
