}
static void ParseRequest( services_discovery_t *p_sd )
{
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    char *psz_request = p_sys->psz_request;
    int i;
    char *psz_tok = strchr( psz_request, ':' );
    if( !psz_tok ) return;
    *psz_tok = '\0';
    if ( ! p_sys->b_savedurls_loaded )
    {
        char *psz_urls = var_GetNonEmptyString( p_sd->p_parent,
                                                "podcast-urls" );
        ParseUrls( p_sd, psz_urls );
        free( psz_urls );
    }
    if( !strcmp( psz_request, "ADD" ) )
    {
        psz_request = psz_tok + 1;
        for( i = 0; i<p_sys->i_urls; i++ )
            if( !strcmp(p_sys->ppsz_urls[i],psz_request) )
              break;
        if( i == p_sys->i_urls )
        {
            INSERT_ELEM( p_sys->ppsz_urls, p_sys->i_urls, p_sys->i_urls,
              strdup( psz_request ) );
            input_item_t *p_input;
            p_input = input_item_New( psz_request, psz_request );
            input_item_AddOption( p_input, "demux=podcast", VLC_INPUT_OPTION_TRUSTED );
            INSERT_ELEM( p_sys->pp_items, p_sys->i_items, p_sys->i_items, p_input );
            services_discovery_AddItem( p_sd, p_input, NULL /* no cat */ );
            INSERT_ELEM( p_sys->pp_input, p_sys->i_input, p_sys->i_input,
                         input_CreateAndStart( p_sd, p_input, NULL ) );
            SaveUrls( p_sd );
        }
    }
    else if ( !strcmp( psz_request, "RM" ) )
    {
        psz_request = psz_tok + 1;
        for( i = 0; i<p_sys->i_urls; i++ )
          if( !strcmp(p_sys->ppsz_urls[i],psz_request) )
            break;
        if( i != p_sys->i_urls )
        {
            services_discovery_RemoveItem( p_sd, p_sys->pp_items[i] );
            vlc_gc_decref( p_sys->pp_items[i] );
            REMOVE_ELEM( p_sys->ppsz_urls, p_sys->i_urls, i );
            REMOVE_ELEM( p_sys->pp_items, p_sys->i_items, i );
        }
        SaveUrls( p_sd );
    }
    free( p_sys->psz_request );
    p_sys->psz_request = NULL;
}
