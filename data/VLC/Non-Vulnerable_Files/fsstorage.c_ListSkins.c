}
static int ListSkins( addons_finder_t *p_finder )
{
    char *psz_dir = getAddonInstallDir( ADDON_SKIN2 );
    if ( !psz_dir )
        return VLC_EGENERIC;
    char **ppsz_list = NULL;
    int i_count = vlc_scandir( psz_dir, &ppsz_list, ListSkin_filter, NULL );
    for ( int i=0; i< i_count; i++ )
    {
        char *psz_file = ppsz_list[i];
        if( !psz_file )
            break;
        if ( FileBelongsToManagedAddon( p_finder, ADDON_SKIN2, psz_file ) )
             continue;
        char *psz_uri;
        if( asprintf( &psz_uri, "unzip://%s/%s!/theme.xml", psz_dir, psz_file ) >= 0)
        {
            int i_ret;
            char *psz_name = NULL;
            char *psz_source = NULL;
            stream_t *p_stream = stream_UrlNew( p_finder, psz_uri );
            free( psz_uri );
            if ( !p_stream )
            {
                i_ret = VLC_EGENERIC;
            }
            else
            {
                i_ret = ParseSkins2Info( p_finder, p_stream, &psz_name, &psz_source );
                if ( i_ret != VLC_SUCCESS )
                {
                    free( psz_name );
                    free( psz_source );
                }
                stream_Delete( p_stream );
            }
            addon_entry_t *p_entry = addon_entry_New();
            p_entry->e_type = ADDON_SKIN2;
            p_entry->e_state = ADDON_INSTALLED;
            if ( i_ret == VLC_SUCCESS )
            {
                p_entry->psz_name = psz_name;
                p_entry->psz_description = strdup("Skins2 theme");
                p_entry->psz_source_uri = psz_source;
            }
            else
            {
                p_entry->e_flags |= ADDON_BROKEN;
                p_entry->psz_name = strdup(psz_file);
                p_entry->psz_description = strdup("Skins2 theme");
            }
            ARRAY_APPEND( p_finder->entries, p_entry );
        }
        free( psz_file );
    }
    free( ppsz_list );
    free( psz_dir );
    return VLC_SUCCESS;
}
