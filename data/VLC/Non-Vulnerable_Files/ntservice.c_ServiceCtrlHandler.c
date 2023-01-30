}
static void WINAPI ServiceCtrlHandler( DWORD control )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_global_intf;
    intf_sys_t    *p_sys  = p_intf->p_sys;
    switch( control )
    {
    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        p_sys->status.dwCurrentState = SERVICE_STOPPED;
        p_sys->status.dwWin32ExitCode = 0;
        p_sys->status.dwCheckPoint = 0;
        p_sys->status.dwWaitHint = 0;
        break;
    case SERVICE_CONTROL_INTERROGATE:
        /* just set the current state to whatever it is... */
        break;
    }
    SetServiceStatus( p_sys->hStatus, &p_sys->status );
}
