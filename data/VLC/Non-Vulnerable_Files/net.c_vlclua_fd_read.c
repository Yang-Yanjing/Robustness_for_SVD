}
static int vlclua_fd_read( lua_State *L )
{
    int fd = vlclua_fd_get( L, luaL_checkint( L, 1 ) );
    size_t i_len = luaL_optint( L, 2, 1 );
    char psz_buffer[i_len];
    ssize_t i_ret = (fd != -1) ? read( fd, psz_buffer, i_len ) : -1;
    if( i_ret > 0 )
        lua_pushlstring( L, psz_buffer, i_ret );
    else
        lua_pushnil( L );
    return 1;
}