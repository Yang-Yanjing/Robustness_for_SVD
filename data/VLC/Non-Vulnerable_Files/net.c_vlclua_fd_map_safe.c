}
static int vlclua_fd_map_safe( lua_State *L, int fd )
{
    int luafd = vlclua_fd_map( L, fd );
    if( luafd == -1 )
        net_Close( fd );
    return luafd;
}
