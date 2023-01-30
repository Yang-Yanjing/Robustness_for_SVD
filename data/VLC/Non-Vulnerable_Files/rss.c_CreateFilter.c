 *****************************************************************************/
static int CreateFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_urls;
    int i_ttl;
    /* Allocate structure */
    p_sys = p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    /* Get the urls to parse: must be non empty */
    psz_urls = var_CreateGetNonEmptyString( p_filter, CFG_PREFIX "urls" );
    if( !psz_urls )
    {
        msg_Err( p_filter, "The list of urls must not be empty" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* Fill the p_sys structure with the configuration */
    p_sys->i_title = var_CreateGetInteger( p_filter, CFG_PREFIX "title" );
    p_sys->i_cur_feed = 0;
    p_sys->i_cur_item = p_sys->i_title == scroll_title ? -1 : 0;
    p_sys->i_cur_char = 0;
    p_sys->i_feeds = 0;
    p_sys->p_feeds = NULL;
    p_sys->i_speed = var_CreateGetInteger( p_filter, CFG_PREFIX "speed" );
    p_sys->i_length = var_CreateGetInteger( p_filter, CFG_PREFIX "length" );
    p_sys->b_images = var_CreateGetBool( p_filter, CFG_PREFIX "images" );
    i_ttl = __MAX( 0, var_CreateGetInteger( p_filter, CFG_PREFIX "ttl" ) );
    p_sys->psz_marquee = malloc( p_sys->i_length + 1 );
    if( p_sys->psz_marquee == NULL )
    {
        free( psz_urls );
        free( p_sys );
        return VLC_ENOMEM;
    }
    p_sys->psz_marquee[p_sys->i_length] = '\0';
    p_sys->p_style = text_style_New();
    if( p_sys->p_style == NULL )
        goto error;
    p_sys->i_xoff = var_CreateGetInteger( p_filter, CFG_PREFIX "x" );
    p_sys->i_yoff = var_CreateGetInteger( p_filter, CFG_PREFIX "y" );
    p_sys->i_pos = var_CreateGetInteger( p_filter, CFG_PREFIX "position" );
    p_sys->p_style->i_font_alpha = var_CreateGetInteger( p_filter, CFG_PREFIX "opacity" );
    p_sys->p_style->i_font_color = var_CreateGetInteger( p_filter, CFG_PREFIX "color" );
    p_sys->p_style->i_font_size = var_CreateGetInteger( p_filter, CFG_PREFIX "size" );
    if( p_sys->b_images && p_sys->p_style->i_font_size == -1 )
    {
        msg_Warn( p_filter, "rss-size wasn't specified. Feed images will thus "
                            "be displayed without being resized" );
    }
    /* Parse the urls */
    if( ParseUrls( p_filter, psz_urls ) )
        goto error;
    /* Misc init */
    vlc_mutex_init( &p_sys->lock );
    p_filter->pf_sub_source = Filter;
    p_sys->last_date = (mtime_t)0;
    p_sys->b_fetched = false;
    /* Create and arm the timer */
    if( vlc_timer_create( &p_sys->timer, Fetch, p_filter ) )
    {
        vlc_mutex_destroy( &p_sys->lock );
        goto error;
    }
    vlc_timer_schedule( p_sys->timer, false, 1,
                        (mtime_t)(i_ttl)*1000000 );
    free( psz_urls );
    return VLC_SUCCESS;
error:
    if( p_sys->p_style )
        text_style_Delete( p_sys->p_style );
    free( p_sys->psz_marquee );
    free( psz_urls );
    free( p_sys );
    return VLC_ENOMEM;
}
