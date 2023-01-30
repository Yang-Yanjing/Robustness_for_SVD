 *****************************************************************************/
static void input_item_add_subitem_tree ( const vlc_event_t * p_event,
                                          void * user_data )
{
    input_item_t *p_input = p_event->p_obj;
    playlist_t *p_playlist = (( playlist_item_t* ) user_data)->p_playlist;
    input_item_node_t *p_new_root = p_event->u.input_item_subitem_tree_added.p_root;
    PL_LOCK;
    playlist_item_t *p_item =
        playlist_ItemGetByInput( p_playlist, p_input );
    assert( p_item != NULL );
    bool b_current = get_current_status_item( p_playlist ) == p_item;
    bool b_autostart = var_GetBool( p_playlist, "playlist-autostart" );
    bool b_stop = p_item->i_flags & PLAYLIST_SUBITEM_STOP_FLAG;
    bool b_flat = false;
    p_item->i_flags &= ~PLAYLIST_SUBITEM_STOP_FLAG;
    /* We will have to flatten the tree out if we are in "the playlist" node and
    the user setting demands flat playlist */
    if( !pl_priv(p_playlist)->b_tree ) {
        playlist_item_t *p_up = p_item;
        while( p_up->p_parent )
        {
            if( p_up->p_parent == p_playlist->p_playing )
            {
                b_flat = true;
                break;
            }
            p_up = p_up->p_parent;
        }
    }
    int pos = 0;
    /* If we have to flatten out, then take the item's position in the parent as
    insertion point and delete the item */
    if( b_flat )
    {
        playlist_item_t *p_parent = p_item->p_parent;
        assert( p_parent != NULL );
        int i;
        for( i = 0; i < p_parent->i_children; i++ )
        {
            if( p_parent->pp_children[i] == p_item )
            {
                pos = i;
                break;
            }
        }
        assert( i < p_parent->i_children );
        playlist_DeleteItem( p_playlist, p_item, true );
        p_item = p_parent;
    }
    else
    {
        pos = p_item->i_children >= 0 ? p_item->i_children : 0;
    }
    /* At this point:
    "p_item" is the node where sub-items should be inserted,
    "pos" is the insertion position in that node */
    int last_pos = playlist_InsertInputItemTree( p_playlist,
                                                 p_item,
                                                 p_new_root,
                                                 pos,
                                                 b_flat );
    if( !b_flat ) var_SetInteger( p_playlist, "leaf-to-parent", p_item->i_id );
    //control playback only if it was the current playing item that got subitems
    if( b_current )
    {
        if( last_pos == pos || ( b_stop && !b_flat ) || !b_autostart )
        {
            /* We stop, either because no sub-item was actually created, or some
            flags/settings want us to do so at this point */
            PL_UNLOCK;
            playlist_Stop( p_playlist );
            return;
        }
        else
        {
            /* Continue to play, either random or the first new item */
            playlist_item_t *p_play_item;
            if( var_GetBool( p_playlist, "random" ) )
            {
                p_play_item = NULL;
            }
            else
            {
                p_play_item = p_item->pp_children[pos];
                /* NOTE: this is a work around the general bug:
                if node-to-be-played contains sub-nodes, then second instead
                of first leaf starts playing (only in case the leafs have just
                been instered and playlist has not yet been rebuilt.)
                */
                while( p_play_item->i_children > 0 )
                    p_play_item = p_play_item->pp_children[0];
            }
            playlist_Control( p_playlist, PLAYLIST_VIEWPLAY,
                                  pl_Locked,
                                  get_current_status_node( p_playlist ),
                                  p_play_item );
        }
    }
    PL_UNLOCK;
}
