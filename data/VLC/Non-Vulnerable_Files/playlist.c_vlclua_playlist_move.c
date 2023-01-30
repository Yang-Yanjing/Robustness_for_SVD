}
static int vlclua_playlist_move( lua_State * L )
{
    int i_item = luaL_checkint( L, 1 );
    int i_target = luaL_checkint( L, 2 );
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    PL_LOCK;
    playlist_item_t *p_item = playlist_ItemGetById( p_playlist, i_item );
    playlist_item_t *p_target = playlist_ItemGetById( p_playlist, i_target );
    if( !p_item || !p_target )
    {
       PL_UNLOCK;
       return vlclua_push_ret( L, -1 );
    }
    int i_ret;
    if( p_target->i_children != -1 )
        i_ret = playlist_TreeMove( p_playlist, p_item, p_target, 0 );
    else
    	i_ret = playlist_TreeMove( p_playlist, p_item, p_target->p_parent, p_target->i_id - p_target->p_parent->pp_children[0]->i_id + 1 );
    PL_UNLOCK;
    return vlclua_push_ret( L, i_ret );
}
