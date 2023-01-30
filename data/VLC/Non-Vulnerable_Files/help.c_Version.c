 *****************************************************************************/
static void Version( void )
{
    ShowConsole();
    printf(_("VLC version %s (%s)\n"), VERSION_MESSAGE, psz_vlc_changeset);
    printf(_("Compiled by %s on %s (%s)\n"), VLC_CompileBy(),
           VLC_CompileHost(), __DATE__" "__TIME__ );
    printf(_("Compiler: %s\n"), VLC_Compiler());
    fputs(LICENSE_MSG, stdout);
    PauseConsole();
}
