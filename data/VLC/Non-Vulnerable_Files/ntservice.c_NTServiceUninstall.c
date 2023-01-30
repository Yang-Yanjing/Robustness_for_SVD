}
static int NTServiceUninstall( intf_thread_t *p_intf )
{
    intf_sys_t *p_sys  = p_intf->p_sys;
    SC_HANDLE handle = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if( handle == NULL )
    {
        msg_Err( p_intf,
                 "could not connect to Services Control Manager database" );
        return VLC_EGENERIC;
    }
    /* First, open a handle to the service */
    SC_HANDLE service = OpenServiceA( handle, p_sys->psz_service, DELETE );
    if( service == NULL )
    {
        msg_Err( p_intf, "could not open service" );
        CloseServiceHandle( handle );
        return VLC_EGENERIC;
    }
    /* Remove the service */
    if( !DeleteService( service ) )
    {
        msg_Err( p_intf, "could not delete service \"%s\"",
                 p_sys->psz_service );
    }
    else
    {
        msg_Dbg( p_intf, "service deleted successfuly" );
    }
    CloseServiceHandle( service );
    CloseServiceHandle( handle );
    return VLC_SUCCESS;
}
