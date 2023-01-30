}
static void vlclua_fd_unmap_safe( lua_State *L, unsigned idx )
{
    int fd = vlclua_fd_get( L, idx );
    vlclua_fd_unmap( L, idx );
    if( fd != -1 )
        net_Close( fd );
}
