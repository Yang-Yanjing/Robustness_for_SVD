 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys;
    /* Allocate instance and initialize some members */
    p_intf->p_sys = p_sys = malloc( sizeof( intf_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->i_fd = lirc_init( "vlc", 1 );
    if( p_sys->i_fd == -1 )
    {
        msg_Err( p_intf, "lirc initialisation failed" );
        goto error;
    }
    /* We want polling */
    fcntl( p_sys->i_fd, F_SETFL, fcntl( p_sys->i_fd, F_GETFL ) | O_NONBLOCK );
    /* Read the configuration file */
    char *psz_file = var_InheritString( p_intf, "lirc-file" );
    int val = lirc_readconfig( psz_file, &p_sys->config, NULL );
    free( psz_file );
    if( val != 0 )
    {
        msg_Err( p_intf, "failure while reading lirc config" );
        lirc_deinit();
        goto error;
    }
    if( vlc_clone( &p_sys->thread, Run, p_intf, VLC_THREAD_PRIORITY_LOW ) )
    {
        lirc_freeconfig( p_sys->config );
        lirc_deinit();
        goto error;
    }
    return VLC_SUCCESS;
error:
    free( p_sys );
    return VLC_EGENERIC;
}
