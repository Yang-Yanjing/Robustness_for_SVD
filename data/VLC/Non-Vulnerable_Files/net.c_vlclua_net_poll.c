/* Takes a { fd : events } table as first arg and modifies it to { fd : revents } */
static int vlclua_net_poll( lua_State *L )
{
    vlclua_dtable_t *dt = vlclua_get_dtable( L );
    luaL_checktype( L, 1, LUA_TTABLE );
    int i_fds = 1;
    lua_pushnil( L );
    while( lua_next( L, 1 ) )
    {
        i_fds++;
        lua_pop( L, 1 );
    }
    struct pollfd *p_fds = xmalloc( i_fds * sizeof( *p_fds ) );
    int *luafds = xmalloc( i_fds * sizeof( *luafds ) );
    lua_pushnil( L );
    int i = 1;
    p_fds[0].fd = dt->fd[0];
    p_fds[0].events = POLLIN;
    while( lua_next( L, 1 ) )
    {
        luafds[i] = luaL_checkinteger( L, -2 );
        p_fds[i].fd = vlclua_fd_get( L, luafds[i] );
        p_fds[i].events = luaL_checkinteger( L, -1 );
        p_fds[i].events &= POLLIN | POLLOUT | POLLPRI;
        lua_pop( L, 1 );
        i++;
    }
    int i_ret;
    do
        i_ret = poll( p_fds, i_fds, -1 );
    while( i_ret == -1 && errno == EINTR );
    for( i = 1; i < i_fds; i++ )
    {
        lua_pushinteger( L, luafds[i] );
        lua_pushinteger( L, p_fds[i].revents );
        lua_settable( L, 1 );
    }
    lua_pushinteger( L, i_ret );
    if( p_fds[0].revents )
        i_ret = luaL_error( L, "Interrupted." );
    else
        i_ret = 1;
    free( luafds );
    free( p_fds );
    return i_ret;
}
