}
static void Fetch( void *p_data )
{
    filter_t *p_filter = p_data;
    filter_sys_t *p_sys = p_filter->p_sys;
    msg_Dbg( p_filter, "Updating the rss feeds" );
    rss_feed_t *p_feeds = FetchRSS( p_filter );
    if( !p_feeds )
    {
        msg_Err( p_filter, "Unable to fetch the feeds" );
        return;
    }
    rss_feed_t *p_old_feeds = p_sys->p_feeds;
    vlc_mutex_lock( &p_sys->lock );
    /* Update the feeds */
    p_sys->p_feeds = p_feeds;
    p_sys->b_fetched = true;
    /* Set all current info to the original values */
    p_sys->i_cur_feed = 0;
    p_sys->i_cur_item = p_sys->i_title == scroll_title ? -1 : 0;
    p_sys->i_cur_char = 0;
    vlc_mutex_unlock( &p_sys->lock );
    if( p_old_feeds )
        FreeRSS( p_old_feeds, p_sys->i_feeds );
}
