/** Gets the OS file descriptor mapped to a VLC Lua file descriptor */
static int vlclua_fd_get( lua_State *L, unsigned idx )
{
    vlclua_dtable_t *dt = vlclua_get_dtable( L );
    if( idx < 3u )
        return idx;
    idx -= 3;
    return (idx < dt->fdc) ? dt->fdv[idx] : -1;
}
