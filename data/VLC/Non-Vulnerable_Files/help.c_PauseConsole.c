 *****************************************************************************/
static void PauseConsole( void )
{
    if( getenv( "PWD" ) ) return; /* Cygwin shell or Wine */
    utf8_fprintf( stderr, _("\nPress the RETURN key to continue...\n") );
    getchar();
    fclose( stdout );
}
