#endif
int vlclua_fd_init( lua_State *L, vlclua_dtable_t *dt )
{
#ifndef _WIN32
    if( vlc_pipe( dt->fd ) )
        return -1;
#else
    if( vlc_socket_pair( dt->fd ) )
        return -1;
#endif
    dt->fdv = NULL;
    dt->fdc = 0;
    vlclua_set_object( L, vlclua_get_dtable, dt );
    luaopen_net_intf( L );
    return 0;
}
