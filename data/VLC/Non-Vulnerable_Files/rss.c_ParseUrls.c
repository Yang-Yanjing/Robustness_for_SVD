 ***************************************************************************/
static int ParseUrls( filter_t *p_filter, char *psz_urls )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    char *psz_urls2 = psz_urls;
    p_sys->i_feeds = 1;
    /* Count the number of feeds */
    while( *psz_urls )
    {
        if( *psz_urls == '|' )
            p_sys->i_feeds++;
        psz_urls++;
    }
    /* Allocate the structure */
    p_sys->p_feeds = malloc( p_sys->i_feeds * sizeof( rss_feed_t ) );
    if( !p_sys->p_feeds )
        return VLC_ENOMEM;
    /* Loop on all urls and fill in the struct */
    psz_urls = psz_urls2;
    for( int i = 0; i < p_sys->i_feeds; i++ )
    {
        rss_feed_t* p_feed = p_sys->p_feeds + i;
        char *psz_end;
        if( i < p_sys->i_feeds - 1 )
        {
            psz_end = strchr( psz_urls, '|' );
            *psz_end = '\0';
        }
        else
            psz_end = psz_urls;
        p_feed->i_items = 0;
        p_feed->p_items = NULL;
        p_feed->psz_title = NULL;
        p_feed->psz_link = NULL;
        p_feed->psz_description = NULL;
        p_feed->psz_image = NULL;
        p_feed->p_pic = NULL;
        p_feed->psz_url = strdup( psz_urls );
        psz_urls = psz_end + 1;
    }
    return VLC_SUCCESS;
}
