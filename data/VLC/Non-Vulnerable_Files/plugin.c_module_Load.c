}
int module_Load( vlc_object_t *p_this, const char *psz_file,
                 module_handle_t *p_handle, bool lazy )
{
    wchar_t *wfile = ToWide (psz_file);
    if (wfile == NULL)
        return -1;
    module_handle_t handle = NULL;
#if (_WIN32_WINNT >= 0x601)
    DWORD mode;
    if (SetThreadErrorMode (SEM_FAILCRITICALERRORS, &mode) != 0)
#endif
    {
        handle = LoadLibraryExW (wfile, NULL, LoadLibraryFlags );
#if (_WIN32_WINNT >= 0x601)
        SetThreadErrorMode (mode, NULL);
#endif
    }
    free (wfile);
    if( handle == NULL )
    {
        char *psz_err = GetWindowsError();
        msg_Warn( p_this, "cannot load module `%s' (%s)", psz_file, psz_err );
        free( psz_err );
        return -1;
    }
    *p_handle = handle;
    (void) lazy;
    return 0;
}
