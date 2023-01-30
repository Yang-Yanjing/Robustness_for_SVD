 *****************************************************************************/
static void input_item_subitem_tree_added( const vlc_event_t * p_event,
                                      void * user_data )
{
    playlist_t *p_playlist = user_data;
    input_item_node_t *p_root =
        p_event->u.input_item_subitem_tree_added.p_root;
    PL_LOCK;
    playlist_InsertInputItemTree ( p_playlist, p_playlist->p_media_library,
                                   p_root, 0, false );
    PL_UNLOCK;
}
