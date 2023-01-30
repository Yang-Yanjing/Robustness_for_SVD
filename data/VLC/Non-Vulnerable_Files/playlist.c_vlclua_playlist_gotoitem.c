}
static int vlclua_playlist_gotoitem( lua_State * L )
{
    int i_id = luaL_checkint( L, 1 );
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    PL_LOCK;
    int i_ret = playlist_Control( p_playlist, PLAYLIST_VIEWPLAY,
                                  true, NULL,
                                  playlist_ItemGetById( p_playlist, i_id ) );
    PL_UNLOCK;
    return vlclua_push_ret( L, i_ret );
}
