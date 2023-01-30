 */
static void Art( playlist_preparser_t *p_preparser, input_item_t *p_item )
{
    vlc_object_t *obj = p_preparser->object;
    playlist_fetcher_t *p_fetcher = p_preparser->p_fetcher;
    bool b_fetch = false;
    /* If we haven't retrieved enough meta, add to secondary queue
     * which will run the "meta fetchers".
     * This only checks for meta, not for art
     * \todo don't do this for things we won't get meta for, like vids
     */
    vlc_mutex_lock( &p_item->lock );
    if( p_item->p_meta )
    {
        const char *psz_arturl = vlc_meta_Get( p_item->p_meta, vlc_meta_ArtworkURL );
        const char *psz_name = vlc_meta_Get( p_item->p_meta, vlc_meta_Title );
        if( !psz_arturl || ( strncasecmp( psz_arturl, "file://", 7 ) &&
                             strncasecmp( psz_arturl, "attachment://", 13 ) ) )
        {
            msg_Dbg( obj, "meta ok for %s, need to fetch art",
                     psz_name ? psz_name : "(null)" );
            b_fetch = true;
        }
        else
        {
            msg_Dbg( obj, "no fetch required for %s (art currently %s)",
                     psz_name ? psz_name : "(null)",
                     psz_arturl ? psz_arturl : "(null)" );
        }
    }
    vlc_mutex_unlock( &p_item->lock );
    if( b_fetch && p_fetcher )
        playlist_fetcher_Push( p_fetcher, p_item, 0 );
}
