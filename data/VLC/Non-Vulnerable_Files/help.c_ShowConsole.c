 *****************************************************************************/
static void ShowConsole( void )
{
    if( getenv( "PWD" ) ) return; /* Cygwin shell or Wine */
    if( !AllocConsole() ) return;
    /* Use the ANSI code page (e.g. Windows-1252) as expected by the LibVLC
     * Unicode/locale subsystem. By default, we have the obsolecent OEM code
     * page (e.g. CP437 or CP850). */
    SetConsoleOutputCP (GetACP ());
    SetConsoleTitle (TEXT("VLC media player version ") TEXT(PACKAGE_VERSION));
    freopen( "CONOUT$", "w", stderr );
    freopen( "CONIN$", "r", stdin );
    if( freopen( "vlc-help.txt", "wt", stdout ) != NULL )
    {
        fputs( "\xEF\xBB\xBF", stdout );
        fprintf( stderr, _("\nDumped content to vlc-help.txt file.\n") );
    }
    else
        freopen( "CONOUT$", "w", stdout );
}
