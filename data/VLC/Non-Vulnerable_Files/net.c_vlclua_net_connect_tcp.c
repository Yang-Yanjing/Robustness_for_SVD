 *****************************************************************************/
static int vlclua_net_connect_tcp( lua_State *L )
{
    vlc_object_t *p_this = vlclua_get_this( L );
    const char *psz_host = luaL_checkstring( L, 1 );
    int i_port = luaL_checkint( L, 2 );
    int i_fd = net_Connect( p_this, psz_host, i_port, SOCK_STREAM, IPPROTO_TCP );
    lua_pushinteger( L, vlclua_fd_map_safe( L, i_fd ) );
    return 1;
}
