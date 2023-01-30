}
void vlclua_fd_interrupt( vlclua_dtable_t *dt )
{
#ifndef _WIN32
    close( dt->fd[1] );
    dt->fd[1] = -1;
#else
    closesocket( dt->fd[0] );
    dt->fd[0] = -1;
#endif
}
