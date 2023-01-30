}
static void ParseUrls( services_discovery_t *p_sd, char *psz_urls )
{
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    int i_new_items = 0;
    input_item_t **pp_new_items = NULL;
    int i_new_urls = 0;
    char **ppsz_new_urls = NULL;
    p_sys->b_savedurls_loaded = true;
    int i, j;
    for( ;; )
    {
        if( !psz_urls )
            break;
        char *psz_tok = strchr( psz_urls, '|' );
        if( psz_tok ) *psz_tok = '\0';
        for( i = 0; i < p_sys->i_urls; i++ )
            if( !strcmp( psz_urls, p_sys->ppsz_urls[i] ) )
                break;
        if( i == p_sys->i_urls )
        {
            INSERT_ELEM( ppsz_new_urls, i_new_urls, i_new_urls,
                         strdup( psz_urls ) );
            input_item_t *p_input;
            p_input = input_item_New( psz_urls, psz_urls );
            input_item_AddOption( p_input, "demux=podcast", VLC_INPUT_OPTION_TRUSTED );
            INSERT_ELEM( pp_new_items, i_new_items, i_new_items, p_input );
            services_discovery_AddItem( p_sd, p_input, NULL /* no cat */ );
            INSERT_ELEM( p_sys->pp_input, p_sys->i_input, p_sys->i_input,
                         input_CreateAndStart( p_sd, p_input, NULL ) );
        }
        else
        {
            INSERT_ELEM( ppsz_new_urls, i_new_urls, i_new_urls,
                         strdup( p_sys->ppsz_urls[i]) );
            INSERT_ELEM( pp_new_items, i_new_items, i_new_items, p_sys->pp_items[i] );
        }
        if( psz_tok )
            psz_urls = psz_tok+1;
        else
            break;
    }
    /* delete removed items and signal the removal */
    for( i = 0; i<p_sys->i_items; ++i )
    {
        for( j = 0; j < i_new_items; ++j )
            if( pp_new_items[j] == p_sys->pp_items[i] ) break;
        if( j == i_new_items )
        {
            services_discovery_RemoveItem( p_sd, p_sys->pp_items[i] );
            vlc_gc_decref( p_sys->pp_items[i] );
        }
    }
    free( p_sys->pp_items );
    for( int i = 0; i < p_sys->i_urls; i++ )
        free( p_sys->ppsz_urls[i] );
    free( p_sys->ppsz_urls );
    p_sys->ppsz_urls = ppsz_new_urls;
    p_sys->i_urls = i_new_urls;
    p_sys->pp_items = pp_new_items;
    p_sys->i_items = i_new_items;
}
