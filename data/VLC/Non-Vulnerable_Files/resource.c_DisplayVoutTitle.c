}
static void DisplayVoutTitle( input_resource_t *p_resource,
                              vout_thread_t *p_vout )
{
    if( p_resource->p_input == NULL )
        return;
    /* TODO display the title only one time for the same input ? */
    input_item_t *p_item = input_GetItem( p_resource->p_input );
    char *psz_nowplaying = input_item_GetNowPlayingFb( p_item );
    if( psz_nowplaying && *psz_nowplaying )
    {
        vout_DisplayTitle( p_vout, psz_nowplaying );
    }
    else
    {
        char *psz_artist = input_item_GetArtist( p_item );
        char *psz_name = input_item_GetTitle( p_item );
        if( !psz_name || *psz_name == '\0' )
        {
            free( psz_name );
            psz_name = input_item_GetName( p_item );
        }
        if( psz_artist && *psz_artist )
        {
            char *psz_string;
            if( asprintf( &psz_string, "%s - %s", psz_name, psz_artist ) != -1 )
            {
                vout_DisplayTitle( p_vout, psz_string );
                free( psz_string );
            }
        }
        else if( psz_name )
        {
            vout_DisplayTitle( p_vout, psz_name );
        }
        free( psz_name );
        free( psz_artist );
    }
    free( psz_nowplaying );
}
