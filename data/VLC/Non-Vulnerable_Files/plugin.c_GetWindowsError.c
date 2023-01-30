extern DWORD LoadLibraryFlags;
static char *GetWindowsError( void )
{
    wchar_t wmsg[256];
    int i = 0, i_error = GetLastError();
    FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, i_error, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                    wmsg, 256, NULL );
    /* Go to the end of the string */
    while( !wmemchr( L"\r\n\0", wmsg[i], 3 ) )
        i++;
    snwprintf( wmsg + i, 256 - i, L" (error %i)", i_error );
    return FromWide( wmsg );
}
