}
void playlist_preparser_fetcher_Push( playlist_preparser_t *p_preparser,
             input_item_t *p_item, input_item_meta_request_option_t i_options )
{
    if( p_preparser->p_fetcher != NULL )
        playlist_fetcher_Push( p_preparser->p_fetcher, p_item, i_options );
}
