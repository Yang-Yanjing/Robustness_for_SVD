 ***************************************************************************/
static rss_feed_t* FetchRSS( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    stream_t *p_stream;
    xml_t *p_xml;
    xml_reader_t *p_xml_reader;
    int i_feed;
    /* These data are not modified after the creation of the module so we don't
       need to hold the lock */
    int i_feeds = p_sys->i_feeds;
    bool b_images = p_sys->b_images;
    /* Allocate a new structure */
    rss_feed_t *p_feeds = malloc( i_feeds * sizeof( rss_feed_t ) );
    if( !p_feeds )
        return NULL;
    p_xml = xml_Create( p_filter );
    if( !p_xml )
    {
        msg_Err( p_filter, "Failed to open XML parser" );
        free( p_feeds );
        return NULL;
    }
    /* Fetch all feeds and parse them */
    for( i_feed = 0; i_feed < i_feeds; i_feed++ )
    {
        rss_feed_t *p_feed = p_feeds + i_feed;
        rss_feed_t *p_old_feed = p_sys->p_feeds + i_feed;
        /* Initialize the structure */
        p_feed->psz_title = NULL;
        p_feed->psz_description = NULL;
        p_feed->psz_link = NULL;
        p_feed->psz_image = NULL;
        p_feed->p_pic = NULL;
        p_feed->i_items = 0;
        p_feed->p_items = NULL;
        p_feed->psz_url = strdup( p_old_feed->psz_url );
        /* Fetch the feed */
        msg_Dbg( p_filter, "opening %s RSS/Atom feed ...", p_feed->psz_url );
        p_stream = stream_UrlNew( p_filter, p_feed->psz_url );
        if( !p_stream )
        {
            msg_Err( p_filter, "Failed to open %s for reading", p_feed->psz_url );
            p_xml_reader = NULL;
            goto error;
        }
        p_xml_reader = xml_ReaderCreate( p_xml, p_stream );
        if( !p_xml_reader )
        {
            msg_Err( p_filter, "Failed to open %s for parsing", p_feed->psz_url );
            goto error;
        }
        /* Parse the feed */
        if( !ParseFeed( p_filter, p_xml_reader, p_feed ) )
            goto error;
        /* If we have a image: load it if requiere */
        if( b_images && p_feed->psz_image && !p_feed->p_pic )
        {
            p_feed->p_pic = LoadImage( p_filter, p_feed->psz_image );
        }
        msg_Dbg( p_filter, "done with %s RSS/Atom feed", p_feed->psz_url );
        xml_ReaderDelete( p_xml_reader );
        stream_Delete( p_stream );
    }
    xml_Delete( p_xml );
    return p_feeds;
error:
    FreeRSS( p_feeds, i_feed + 1 );
    if( p_xml_reader )
        xml_ReaderDelete( p_xml_reader );
    if( p_stream )
        stream_Delete( p_stream );
    if( p_xml )
        xml_Delete( p_xml );
    return NULL;
}
