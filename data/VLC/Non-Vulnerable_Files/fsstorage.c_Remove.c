}
static int Remove( addons_storage_t *p_storage, addon_entry_t *p_entry )
{
    vlc_mutex_lock( &p_entry->lock );
    FOREACH_ARRAY( addon_file_t *p_file, p_entry->files )
    switch( p_file->e_filetype )
    {
        case ADDON_EXTENSION:
        case ADDON_PLAYLIST_PARSER:
        case ADDON_SERVICE_DISCOVERY:
        case ADDON_INTERFACE:
        case ADDON_META:
        case ADDON_SKIN2:
        {
            char *psz_dest;
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
            free( psz_dir );
            free( psz_translated_filename );
            vlc_unlink( psz_dest );
            msg_Dbg( p_storage, "removing %s", psz_dest );
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
    /* Remove file info on success */
    FOREACH_ARRAY( addon_file_t *p_file, p_entry->files )
    free( p_file->psz_filename );
    free( p_file->psz_download_uri );
    free( p_file );
    FOREACH_END()
    ARRAY_RESET( p_entry->files );
    vlc_mutex_unlock( &p_entry->lock );
    return VLC_SUCCESS;
}
