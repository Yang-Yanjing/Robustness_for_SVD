}
void input_SendEventMetaName( input_thread_t *p_input, const char *psz_name )
{
    Trigger( p_input, INPUT_EVENT_ITEM_NAME );
    /* FIXME remove this ugliness */
    vlc_event_t event;
    event.type = vlc_InputItemNameChanged;
    event.u.input_item_name_changed.new_name = psz_name;
    vlc_event_send( &p_input->p->p_item->event_manager, &event );
}
