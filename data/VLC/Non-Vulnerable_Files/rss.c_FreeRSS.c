 ***************************************************************************/
static void FreeRSS( rss_feed_t *p_feeds, int i_feeds )
{
    for( int i_feed = 0; i_feed < i_feeds; i_feed++ )
    {
        rss_feed_t *p_feed = p_feeds+i_feed;
        for( int i_item = 0; i_item < p_feed->i_items; i_item++ )
        {
            rss_item_t *p_item = p_feed->p_items+i_item;
            free( p_item->psz_title );
            free( p_item->psz_link );
            free( p_item->psz_description );
        }
        free( p_feed->p_items );
        free( p_feed->psz_title);
        free( p_feed->psz_link );
        free( p_feed->psz_description );
        free( p_feed->psz_image );
        if( p_feed->p_pic != NULL )
            picture_Release( p_feed->p_pic );
        free( p_feed->psz_url );
    }
    free( p_feeds );
}
