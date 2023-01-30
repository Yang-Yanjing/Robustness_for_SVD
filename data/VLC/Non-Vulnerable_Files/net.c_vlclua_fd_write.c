#ifndef _WIN32
static int vlclua_fd_write( lua_State *L )
{
    int fd = vlclua_fd_get( L, luaL_checkint( L, 1 ) );
    size_t i_len;
    const char *psz_buffer = luaL_checklstring( L, 2, &i_len );
    i_len = luaL_optint( L, 3, i_len );
    lua_pushinteger( L, (fd != -1) ? write( fd, psz_buffer, i_len ) : -1 );
    return 1;
}
