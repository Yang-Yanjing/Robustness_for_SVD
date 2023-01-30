}
static int vlclua_playlist_enqueue( lua_State *L )
{
    int i_count;
    vlc_object_t *p_this = vlclua_get_this( L );
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    i_count = vlclua_playlist_add_internal( p_this, L, p_playlist,
                                            NULL, false );
    lua_pushinteger( L, i_count );
    return 1;
}
