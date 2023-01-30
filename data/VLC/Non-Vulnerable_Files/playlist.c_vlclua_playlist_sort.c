}
static int vlclua_playlist_sort( lua_State *L )
{
    /* allow setting the different sort keys */
    int i_mode = vlc_sort_key_from_string( luaL_checkstring( L, 1 ) );
    if( i_mode == -1 )
        return luaL_error( L, "Invalid search key." );
    int i_type = luaL_optboolean( L, 2, 0 ) ? ORDER_REVERSE : ORDER_NORMAL;
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    PL_LOCK;
    int i_ret = playlist_RecursiveNodeSort( p_playlist, p_playlist->p_playing,
                                            i_mode, i_type );
    PL_UNLOCK;
    return vlclua_push_ret( L, i_ret );
}
