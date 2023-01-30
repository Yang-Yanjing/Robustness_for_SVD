}
static void uninstall_input_item_observer( playlist_item_t * p_item )
{
    vlc_event_manager_t * p_em = &p_item->p_input->event_manager;
    vlc_event_detach( p_em, vlc_InputItemSubItemTreeAdded,
                      input_item_add_subitem_tree, p_item );
    vlc_event_detach( p_em, vlc_InputItemMetaChanged,
                      input_item_changed, p_item );
    vlc_event_detach( p_em, vlc_InputItemDurationChanged,
                      input_item_changed, p_item );
    vlc_event_detach( p_em, vlc_InputItemNameChanged,
                      input_item_changed, p_item );
    vlc_event_detach( p_em, vlc_InputItemInfoChanged,
                      input_item_changed, p_item );
    vlc_event_detach( p_em, vlc_InputItemErrorWhenReadingChanged,
                      input_item_changed, p_item );
}
