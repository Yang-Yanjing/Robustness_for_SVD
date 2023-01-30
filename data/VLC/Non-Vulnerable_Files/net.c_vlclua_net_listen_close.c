}
static int vlclua_net_listen_close( lua_State *L )
{
    int **ppi_fd = (int**)luaL_checkudata( L, 1, "net_listen" );
    int *pi_fd = *ppi_fd;
    for( unsigned i = 0; pi_fd[i] != -1; i++ )
        vlclua_fd_unmap( L, vlclua_fd_get_lua( L, pi_fd[i] ) );
    net_ListenClose( pi_fd );
    return 0;
}
