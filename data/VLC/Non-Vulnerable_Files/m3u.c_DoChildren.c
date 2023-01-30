 *****************************************************************************/
static void DoChildren( playlist_export_t *p_export, playlist_item_t *p_root,
                        int (*pf_fprintf) (FILE *, const char *, ...) )
{
    /* Write header */
    fputs( "#EXTM3U\n", p_export->p_file );
    /* Go through the playlist and add items */
    for( int i = 0; i< p_root->i_children ; i++)
    {
        playlist_item_t *p_current = p_root->pp_children[i];
        assert( p_current );
        if( p_current->i_flags & PLAYLIST_SAVE_FLAG )
            continue;
        if( p_current->i_children >= 0 )
        {
            DoChildren( p_export, p_current, pf_fprintf );
            continue;
        }
        /* General info */
        char *psz_uri = input_item_GetURI( p_current->p_input );
        assert( psz_uri );
        char *psz_name = input_item_GetName( p_current->p_input );
        if( psz_name && strcmp( psz_uri, psz_name ) )
        {
            char *psz_artist = input_item_GetArtist( p_current->p_input );
            if( psz_artist == NULL ) psz_artist = strdup( "" );
            mtime_t i_duration = input_item_GetDuration( p_current->p_input );
            if( psz_artist && *psz_artist )
            {
                /* write EXTINF with artist */
                pf_fprintf( p_export->p_file, "#EXTINF:%"PRIu64",%s - %s\n",
                            i_duration / CLOCK_FREQ, psz_artist, psz_name);
            }
            else
            {
                /* write EXTINF without artist */
                pf_fprintf( p_export->p_file, "#EXTINF:%"PRIu64",%s\n",
                            i_duration / CLOCK_FREQ, psz_name);
            }
            free( psz_artist );
        }
        free( psz_name );
        /* VLC specific options */
        vlc_mutex_lock( &p_current->p_input->lock );
        for( int j = 0; j < p_current->p_input->i_options; j++ )
        {
            pf_fprintf( p_export->p_file, "#EXTVLCOPT:%s\n",
                        p_current->p_input->ppsz_options[j][0] == ':' ?
                        p_current->p_input->ppsz_options[j] + 1 :
                        p_current->p_input->ppsz_options[j] );
        }
        vlc_mutex_unlock( &p_current->p_input->lock );
        /* Stupid third party players don't understand file: URIs. */
        char *psz_path = make_path( psz_uri );
        if( psz_path != NULL )
        {
            free( psz_uri );
            psz_uri = psz_path;
        }
        fprintf( p_export->p_file, "%s\n", psz_uri );
        free( psz_uri );
    }
}
