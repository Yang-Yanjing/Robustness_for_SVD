}
void input_SendEventMetaInfo( input_thread_t *p_input )
{
    Trigger( p_input, INPUT_EVENT_ITEM_INFO );
    /* FIXME remove this ugliness */
    vlc_event_t event;
    event.type = vlc_InputItemInfoChanged;
    vlc_event_send( &p_input->p->p_item->event_manager, &event );
}
