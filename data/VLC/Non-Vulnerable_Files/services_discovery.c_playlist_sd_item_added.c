 /* A new item has been added to a certain sd */
static void playlist_sd_item_added( const vlc_event_t * p_event, void * user_data )
{
    input_item_t * p_input = p_event->u.services_discovery_item_added.p_new_item;
    const char * psz_cat = p_event->u.services_discovery_item_added.psz_category;
    playlist_item_t * p_parent = user_data;
    playlist_t * p_playlist = p_parent->p_playlist;
    msg_Dbg( p_playlist, "Adding %s in %s",
                p_input->psz_name ? p_input->psz_name : "(null)",
                psz_cat ? psz_cat : "(null)" );
    PL_LOCK;
    /* If p_parent is in root category (this is clearly a hack) and we have a cat */
    if( !EMPTY_STR(psz_cat) )
    {
        /* */
        playlist_item_t * p_cat;
        p_cat = playlist_ChildSearchName( p_parent, psz_cat );
        if( !p_cat )
        {
            p_cat = playlist_NodeCreate( p_playlist, psz_cat,
                                         p_parent, PLAYLIST_END, 0, NULL );
            p_cat->i_flags &= ~PLAYLIST_SKIP_FLAG;
        }
        p_parent = p_cat;
    }
    playlist_NodeAddInput( p_playlist, p_input, p_parent,
                           PLAYLIST_APPEND, PLAYLIST_END,
                           pl_Locked );
    PL_UNLOCK;
}
