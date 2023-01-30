 */
static void DoChildren( playlist_export_t *p_export, playlist_item_t *p_root )
{
    /* Go through the playlist and add items */
    for( int i = 0; i < p_root->i_children ; i++)
    {
        playlist_item_t *p_current = p_root->pp_children[i];
        assert( p_current );
        if( p_current->i_flags & PLAYLIST_SAVE_FLAG )
            continue;
        if( p_current->i_children >= 0 )
        {
            DoChildren( p_export, p_current );
            continue;
        }
        char* psz_name = NULL;
        char *psz_tmp = input_item_GetName( p_current->p_input );
        if( psz_tmp )
            psz_name = convert_xml_special_chars( psz_tmp );
        free( psz_tmp );
        if( psz_name )
        {
            char* psz_artist = NULL;
            psz_tmp = input_item_GetArtist( p_current->p_input );
            if( psz_tmp )
                psz_artist = convert_xml_special_chars( psz_tmp );
            free( psz_tmp );
            mtime_t i_duration = input_item_GetDuration( p_current->p_input );
            int min = ( i_duration / 1000000 ) / 60;
            int sec = ( i_duration / 1000000 ) - min * 60;
            // Print the artist if we have one
            if( psz_artist && *psz_artist )
                fprintf( p_export->p_file, "    <li>%s - %s (%02d:%02d)</li>\n", psz_artist, psz_name, min, sec );
            else
                fprintf( p_export->p_file, "    <li>%s (%2d:%2d)</li>\n", psz_name, min, sec );
            free( psz_artist );
        }
        free( psz_name );
    }
}
