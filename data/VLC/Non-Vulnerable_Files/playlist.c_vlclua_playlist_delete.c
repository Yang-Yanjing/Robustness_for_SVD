}
static int vlclua_playlist_delete( lua_State * L )
{
    int i_id = luaL_checkint( L, 1 );
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    PL_LOCK;
    playlist_item_t *p_item = playlist_ItemGetById( p_playlist, i_id );
    if( !p_item )
    {
       PL_UNLOCK;
       return vlclua_push_ret( L, -1 );
    }
    int i_ret = playlist_DeleteFromInput( p_playlist, p_item -> p_input, true );
    PL_UNLOCK;
    return vlclua_push_ret( L, i_ret );
}
