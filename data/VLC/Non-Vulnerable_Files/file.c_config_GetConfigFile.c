 */
static char *config_GetConfigFile( vlc_object_t *obj )
{
    char *psz_file = var_CreateGetNonEmptyString( obj, "config" );
    var_Destroy( obj, "config" );
    if( psz_file == NULL )
    {
        char *psz_dir = config_GetUserDir( VLC_CONFIG_DIR );
        if( asprintf( &psz_file, "%s" DIR_SEP CONFIG_FILE, psz_dir ) == -1 )
            psz_file = NULL;
        free( psz_dir );
    }
    return psz_file;
}
