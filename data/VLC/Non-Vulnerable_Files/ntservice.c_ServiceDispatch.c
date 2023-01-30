}
static void WINAPI ServiceDispatch( DWORD numArgs, char **args )
{
    (void)numArgs;
    (void)args;
    intf_thread_t *p_intf = (intf_thread_t *)p_global_intf;
    intf_sys_t    *p_sys  = p_intf->p_sys;
    char *psz_modules, *psz_parser;
    /* We have to initialize the service-specific stuff */
    memset( &p_sys->status, 0, sizeof(SERVICE_STATUS) );
    p_sys->status.dwServiceType = SERVICE_WIN32;
    p_sys->status.dwCurrentState = SERVICE_START_PENDING;
    p_sys->status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    p_sys->hStatus =
        RegisterServiceCtrlHandlerA( p_sys->psz_service, &ServiceCtrlHandler );
    if( p_sys->hStatus == (SERVICE_STATUS_HANDLE)0 )
    {
        msg_Err( p_intf, "failed to register service control handler" );
        return;
    }
    /*
     * Load background interfaces
     */
    psz_modules = var_InheritString( p_intf, "ntservice-extraintf" );
    psz_parser = psz_modules;
    while( psz_parser && *psz_parser )
    {
        char *psz_module, *psz_temp;
        psz_module = psz_parser;
        psz_parser = strchr( psz_module, ',' );
        if( psz_parser )
        {
            *psz_parser = '\0';
            psz_parser++;
        }
        if( asprintf( &psz_temp, "%s,none", psz_module ) != -1 )
        {
            /* Try to create the interface */
            if( intf_Create( pl_Get(p_intf), psz_temp ) )
            {
                msg_Err( p_intf, "interface \"%s\" initialization failed",
                         psz_temp );
                free( psz_temp );
                continue;
            }
            free( psz_temp );
        }
    }
    free( psz_modules );
    /* Initialization complete - report running status */
    p_sys->status.dwCurrentState = SERVICE_RUNNING;
    p_sys->status.dwCheckPoint   = 0;
    p_sys->status.dwWaitHint     = 0;
    SetServiceStatus( p_sys->hStatus, &p_sys->status );
}
