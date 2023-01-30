 *****************************************************************************/
static void *Run( void *data )
{
    intf_thread_t *p_intf = data;
    intf_sys_t *p_sys = p_intf->p_sys;
    struct pollfd ufd;
    ufd.fd = p_sys->i_fd;
    ufd.events = POLLIN;
    for( ;; )
    {
        /* Wait for data */
        if( poll( &ufd, 1, -1 ) == -1 )
        {
            if( errno == EINTR )
                continue;
            break;
        }
        /* Process */
        int canc = vlc_savecancel();
        Process( p_intf );
        vlc_restorecancel(canc);
    }
    return NULL;
}
