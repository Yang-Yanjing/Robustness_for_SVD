/** Maps an OS file descriptor to a VLC Lua file descriptor */
static int vlclua_fd_map( lua_State *L, int fd )
{
    vlclua_dtable_t *dt = vlclua_get_dtable( L );
    if( (unsigned)fd < 3u )
        return -1;
#ifndef NDEBUG
    for( unsigned i = 0; i < dt->fdc; i++ )
        assert( dt->fdv[i] != fd );
#endif
    for( unsigned i = 0; i < dt->fdc; i++ )
    {
        if( dt->fdv[i] == -1 )
        {
            dt->fdv[i] = fd;
            return 3 + i;
        }
    }
    if( dt->fdc >= 64 )
        return -1;
    int *fdv = realloc( dt->fdv, (dt->fdc + 1) * sizeof (dt->fdv[0]) );
    if( unlikely(fdv == NULL) )
        return -1;
    dt->fdv = fdv;
    dt->fdv[dt->fdc] = fd;
    fd = 3 + dt->fdc;
    dt->fdc++;
    return fd;
}
