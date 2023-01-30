/* A request to remove all ideas from SD */
static void playlist_sd_item_removeall( const vlc_event_t * p_event, void * user_data )
{
    VLC_UNUSED(p_event);
    playlist_item_t* p_sd_node = user_data;
    if( p_sd_node == NULL ) return;
    playlist_t* p_playlist = p_sd_node->p_playlist;
    PL_LOCK;
    playlist_NodeEmpty( p_playlist, p_sd_node, true );
    PL_UNLOCK;
}
