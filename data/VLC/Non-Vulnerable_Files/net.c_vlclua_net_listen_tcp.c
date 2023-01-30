};
static int vlclua_net_listen_tcp( lua_State *L )
{
    vlc_object_t *p_this = vlclua_get_this( L );
    const char *psz_host = luaL_checkstring( L, 1 );
    int i_port = luaL_checkint( L, 2 );
    int *pi_fd = net_ListenTCP( p_this, psz_host, i_port );
    if( pi_fd == NULL )
        return luaL_error( L, "Cannot listen on %s:%d", psz_host, i_port );
    for( unsigned i = 0; pi_fd[i] != -1; i++ )
        if( vlclua_fd_map( L, pi_fd[i] ) == -1 )
        {
            while( i > 0 )
                vlclua_fd_unmap( L, vlclua_fd_get_lua( L, pi_fd[--i] ) );
            net_ListenClose( pi_fd );
            return luaL_error( L, "Cannot listen on %s:%d", psz_host, i_port );
        }
    int **ppi_fd = lua_newuserdata( L, sizeof( int * ) );
    *ppi_fd = pi_fd;
    if( luaL_newmetatable( L, "net_listen" ) )
    {
        lua_newtable( L );
        luaL_register( L, NULL, vlclua_net_listen_reg );
        lua_setfield( L, -2, "__index" );
        lua_pushcfunction( L, vlclua_net_listen_close );
        lua_setfield( L, -2, "__gc" );
    }
    lua_setmetatable( L, -2 );
    return 1;
}
