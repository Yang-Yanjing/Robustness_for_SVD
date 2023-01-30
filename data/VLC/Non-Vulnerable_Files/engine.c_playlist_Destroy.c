 */
void playlist_Destroy( playlist_t *p_playlist )
{
    playlist_private_t *p_sys = pl_priv(p_playlist);
    /* Remove all services discovery */
    playlist_ServicesDiscoveryKillAll( p_playlist );
    msg_Dbg( p_playlist, "destroying" );
    playlist_Deactivate( p_playlist );
    if( p_sys->p_preparser )
        playlist_preparser_Delete( p_sys->p_preparser );
    /* Release input resources */
    assert( p_sys->p_input == NULL );
    input_resource_Release( p_sys->p_input_resource );
    if( p_playlist->p_media_library != NULL )
        playlist_MLDump( p_playlist );
    PL_LOCK;
    /* Release the current node */
    set_current_status_node( p_playlist, NULL );
    /* Release the current item */
    set_current_status_item( p_playlist, NULL );
    PL_UNLOCK;
    vlc_cond_destroy( &p_sys->signal );
    vlc_mutex_destroy( &p_sys->lock );
    /* Remove all remaining items */
    FOREACH_ARRAY( playlist_item_t *p_del, p_playlist->all_items )
        free( p_del->pp_children );
        vlc_gc_decref( p_del->p_input );
        free( p_del );
    FOREACH_END();
    ARRAY_RESET( p_playlist->all_items );
    FOREACH_ARRAY( playlist_item_t *p_del, p_sys->items_to_delete )
        free( p_del->pp_children );
        vlc_gc_decref( p_del->p_input );
        free( p_del );
    FOREACH_END();
    ARRAY_RESET( p_sys->items_to_delete );
    ARRAY_RESET( p_playlist->items );
    ARRAY_RESET( p_playlist->current );
    vlc_object_release( p_playlist );
}
