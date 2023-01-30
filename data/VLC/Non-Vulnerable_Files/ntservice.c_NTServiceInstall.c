 *****************************************************************************/
static int NTServiceInstall( intf_thread_t *p_intf )
{
    intf_sys_t *p_sys  = p_intf->p_sys;
    char psz_path[10*MAX_PATH], *psz_extra;
    TCHAR psz_pathtmp[MAX_PATH];
    SC_HANDLE handle = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if( handle == NULL )
    {
        msg_Err( p_intf,
                 "could not connect to Services Control Manager database" );
        return VLC_EGENERIC;
    }
    /* Find out the filename of ourselves so we can install it to the
     * service control manager */
    GetModuleFileName( NULL, psz_pathtmp, MAX_PATH );
    sprintf( psz_path, "\"%s\" -I "MODULE_STRING, FromT(psz_pathtmp) );
    psz_extra = var_InheritString( p_intf, "ntservice-extraintf" );
    if( psz_extra )
    {
        strcat( psz_path, " --ntservice-extraintf " );
        strcat( psz_path, psz_extra );
        free( psz_extra );
    }
    psz_extra = var_InheritString( p_intf, "ntservice-options" );
    if( psz_extra && *psz_extra )
    {
        strcat( psz_path, " " );
        strcat( psz_path, psz_extra );
        free( psz_extra );
    }
    SC_HANDLE service =
        CreateServiceA( handle, p_sys->psz_service, p_sys->psz_service,
                       GENERIC_READ | GENERIC_EXECUTE,
                       SERVICE_WIN32_OWN_PROCESS,
                       SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
                       psz_path, NULL, NULL, NULL, NULL, NULL );
    if( service == NULL )
    {
        if( GetLastError() != ERROR_SERVICE_EXISTS )
        {
            msg_Err( p_intf, "could not create new service: \"%s\" (%s)",
                     p_sys->psz_service ,psz_path );
            CloseServiceHandle( handle );
            return VLC_EGENERIC;
        }
        else
        {
            msg_Warn( p_intf, "service \"%s\" already exists",
                      p_sys->psz_service );
        }
    }
    else
    {
        msg_Warn( p_intf, "service successfuly created" );
    }
    if( service ) CloseServiceHandle( service );
    CloseServiceHandle( handle );
    return VLC_SUCCESS;
}
