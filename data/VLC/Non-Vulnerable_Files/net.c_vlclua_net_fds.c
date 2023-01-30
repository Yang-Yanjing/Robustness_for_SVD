}
static int vlclua_net_fds( lua_State *L )
{
    int **ppi_fd = (int**)luaL_checkudata( L, 1, "net_listen" );
    int *pi_fd = *ppi_fd;
    int i_count = 0;
    while( pi_fd[i_count] != -1 )
        lua_pushinteger( L, vlclua_fd_get_lua( L, pi_fd[i_count++] ) );
    return i_count;
}
