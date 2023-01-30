 */
static void FetchMeta( playlist_fetcher_t *p_fetcher, input_item_t *p_item )
{
    meta_fetcher_t *p_finder =
        vlc_custom_create( p_fetcher->object, sizeof( *p_finder ), "art finder" );
    if ( !p_finder )
        return;
    p_finder->e_scope = p_fetcher->e_scope;
    p_finder->p_item = p_item;
    module_t *p_module = module_need( p_finder, "meta fetcher", NULL, false );
    if( p_module )
        module_unneed( p_finder, p_module );
    vlc_object_release( p_finder );
}
