  "\n");
static void Help (vlc_object_t *p_this, char const *psz_help_name)
{
    ShowConsole();
    if( psz_help_name && !strcmp( psz_help_name, "help" ) )
    {
        printf(_(vlc_usage), "vlc");
        Usage( p_this, "=core" );
        print_help_on_full_help();
    }
    else if( psz_help_name && !strcmp( psz_help_name, "longhelp" ) )
    {
        printf(_(vlc_usage), "vlc");
        Usage( p_this, NULL );
        print_help_on_full_help();
    }
    else if( psz_help_name && !strcmp( psz_help_name, "full-help" ) )
    {
        printf(_(vlc_usage), "vlc");
        Usage( p_this, NULL );
    }
    else if( psz_help_name )
    {
        Usage( p_this, psz_help_name );
    }
    PauseConsole();
}
