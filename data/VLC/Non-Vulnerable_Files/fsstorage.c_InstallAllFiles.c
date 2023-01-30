}
static int InstallAllFiles( addons_storage_t *p_this, const addon_entry_t *p_entry )
{
    const addon_file_t *p_file;
    char *psz_dest;
    if ( p_entry->files.i_size < 1 )
        return VLC_EGENERIC;
    FOREACH_ARRAY( p_file, p_entry->files )
    switch( p_file->e_filetype )
    {
        case ADDON_EXTENSION:
        case ADDON_PLAYLIST_PARSER:
        case ADDON_SERVICE_DISCOVERY:
        case ADDON_INTERFACE:
        case ADDON_META:
        case ADDON_SKIN2:
        {
            if ( strstr( p_file->psz_filename, ".." ) )
                return VLC_EGENERIC;
            char *psz_translated_filename = strdup( p_file->psz_filename );
            if ( !psz_translated_filename )
                return VLC_ENOMEM;
            char *tmp = psz_translated_filename;
            while (*tmp++) if ( *tmp == '/' ) *tmp = DIR_SEP_CHAR;
            char *psz_dir = getAddonInstallDir( p_file->e_filetype );
            if ( !psz_dir || asprintf( &psz_dest, "%s"DIR_SEP"%s", psz_dir,
                           psz_translated_filename ) < 1 )
            {
                free( psz_dir );
                free( psz_translated_filename );
                return VLC_EGENERIC;
            }
            free( psz_translated_filename );
            free( psz_dir );
            if ( InstallFile( p_this, p_file->psz_download_uri, psz_dest ) != VLC_SUCCESS )
            {
                free( psz_dest );
                return VLC_EGENERIC;
            }
            free( psz_dest );
            break;
        }
        /* Ignore all other unhandled files */
        case ADDON_UNKNOWN:
        case ADDON_PLUGIN:
        case ADDON_OTHER:
        default:
            break;
    }
    FOREACH_END()
    return VLC_SUCCESS;
}
