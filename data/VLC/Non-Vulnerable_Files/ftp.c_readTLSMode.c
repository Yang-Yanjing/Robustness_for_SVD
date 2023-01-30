static int ftp_StopStream ( vlc_object_t *, access_sys_t * );
static void readTLSMode( access_sys_t *p_sys, const char * psz_access )
{
    if ( !strncmp( psz_access, "ftps", 4 ) )
        p_sys->tlsmode = IMPLICIT;
    else
    if ( !strncmp( psz_access, "ftpes", 5 ) )
        p_sys->tlsmode = EXPLICIT;
    else
        p_sys->tlsmode = NONE;
}
