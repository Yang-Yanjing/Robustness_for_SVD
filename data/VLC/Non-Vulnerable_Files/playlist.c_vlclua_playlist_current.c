}
static int vlclua_playlist_current( lua_State *L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    input_thread_t *p_input = playlist_CurrentInput( p_playlist );
    int id = -1;
    if( p_input )
    {
        input_item_t *p_item = input_GetItem( p_input );
        if( p_item )
            id = p_item->i_id;
        vlc_object_release( p_input );
    }
#warning Indexing input items by ID is unsafe,
    lua_pushinteger( L, id );
    return 1;
}
