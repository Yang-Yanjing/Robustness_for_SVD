/** Releases all (leaked) VLC Lua file descriptors. */
void vlclua_fd_cleanup( vlclua_dtable_t *dt )
{
    for( unsigned i = 0; i < dt->fdc; i++ )
        if( dt->fdv[i] != -1 )
            net_Close( dt->fdv[i] );
    free( dt->fdv );
#ifndef _WIN32
    if( dt->fd[1] != -1 )
        close( dt->fd[1] );
    close( dt->fd[0] );
#else
    if( dt->fd[0] != -1 )
        closesocket( dt->fd[0] );
#endif
}
