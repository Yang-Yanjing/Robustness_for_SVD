}
static int ListScripts( addons_finder_t *p_finder, addon_type_t type )
{
    char *psz_dir = getAddonInstallDir( type );
    if ( ! psz_dir ) return VLC_EGENERIC;
    char **ppsz_list = NULL;
    int i_count = vlc_scandir( psz_dir, &ppsz_list, ListScript_filter, NULL );
    for ( int i=0; i< i_count; i++ )
    {
        char *psz_file = ppsz_list[i];
        if( !psz_file )
            break;
        if ( FileBelongsToManagedAddon( p_finder, type, psz_file ) )
             continue;
        addon_entry_t *p_entry = addon_entry_New();
        p_entry->e_state = ADDON_INSTALLED;
        p_entry->e_type = type;
        p_entry->e_flags |= ADDON_BROKEN;
        p_entry->psz_name = strdup(psz_file);
        p_entry->psz_description = strdup("Lua script");
        ARRAY_APPEND( p_finder->entries, p_entry );
        free( psz_file );
    }
    free( ppsz_list );
    free( psz_dir );
    return VLC_SUCCESS;
}
