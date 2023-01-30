}
static int vlclua_action_id( lua_State *L )
{
    vlc_action_t i_key = vlc_GetActionId( luaL_checkstring( L, 1 ) );
    if (i_key == 0)
        return 0;
    lua_pushnumber( L, i_key );
    return 1;
}
