vlc_module_end ()
static char * getAddonInstallDir( addon_type_t t )
{
    const char *psz_subdir = NULL;
    char *psz_dir;
    char *psz_userdir = config_GetUserDir( VLC_DATA_DIR );
    if ( !psz_userdir ) return NULL;
    for ( unsigned int i=0; i< ARRAY_SIZE(addons_dirs); i++ )
    {
        if ( addons_dirs[i].t == t )
        {
            psz_subdir = addons_dirs[i].psz_dir;
            break;
        }
    }
    if ( !psz_subdir )
    {
        free ( psz_userdir );
        return NULL;
    }
    if ( asprintf( &psz_dir, "%s%s", psz_userdir, psz_subdir ) < 1 )
    {
        free( psz_userdir );
        return NULL;
    }
    free( psz_userdir );
    return psz_dir;
}
