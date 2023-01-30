/** Gets the VLC Lua file descriptor mapped from an OS file descriptor */
static int vlclua_fd_get_lua( lua_State *L, int fd )
{
    vlclua_dtable_t *dt = vlclua_get_dtable( L );
    if( (unsigned)fd < 3u )
        return fd;
    for( unsigned i = 0; i < dt->fdc; i++ )
        if( dt->fdv[i] == fd )
            return 3 + i;
    return -1;
}
