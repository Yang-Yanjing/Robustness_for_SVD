/** Unmaps an OS file descriptor from VLC Lua */
static void vlclua_fd_unmap( lua_State *L, unsigned idx )
{
    vlclua_dtable_t *dt = vlclua_get_dtable( L );
    int fd = -1;
    if( idx < 3u )
        return; /* Never close stdin/stdout/stderr. */
    idx -= 3;
    if( idx >= dt->fdc )
        return;
    fd = dt->fdv[idx];
    dt->fdv[idx] = -1;
    while( dt->fdc > 0 && dt->fdv[dt->fdc - 1] == -1 )
        dt->fdc--;
    /* realloc() not really needed */
#ifndef NDEBUG
    for( unsigned i = 0; i < dt->fdc; i++ )
        assert( dt->fdv[i] != fd );
#endif
}
