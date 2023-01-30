 *****************************************************************************/
static void *Run( void *data )
{
    intf_thread_t *p_intf = data;
    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT(VLCSERVICENAME), &ServiceDispatch },
        { NULL, NULL }
    };
    p_global_intf = p_intf;
    p_intf->p_sys->psz_service = var_InheritString( p_intf, "ntservice-name" );
    p_intf->p_sys->psz_service = p_intf->p_sys->psz_service ?
        p_intf->p_sys->psz_service : strdup(VLCSERVICENAME);
    if( var_InheritBool( p_intf, "ntservice-install" ) )
    {
        NTServiceInstall( p_intf );
        return NULL;
    }
    if( var_InheritBool( p_intf, "ntservice-uninstall" ) )
    {
        NTServiceUninstall( p_intf );
        return NULL;
    }
    if( StartServiceCtrlDispatcher( dispatchTable ) == 0 )
    {
        msg_Err( p_intf, "StartServiceCtrlDispatcher failed" ); /* str review */
    }
    free( p_intf->p_sys->psz_service );
    /* Make sure we exit (In case other interfaces have been spawned) */
    libvlc_Quit( p_intf->p_libvlc );
    return NULL;
}
