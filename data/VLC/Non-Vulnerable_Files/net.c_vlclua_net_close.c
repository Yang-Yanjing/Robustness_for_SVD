}
static int vlclua_net_close( lua_State *L )
{
    int i_fd = luaL_checkint( L, 1 );
    vlclua_fd_unmap_safe( L, i_fd );
    return 0;
}
