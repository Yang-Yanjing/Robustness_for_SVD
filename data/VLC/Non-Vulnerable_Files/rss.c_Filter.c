 ****************************************************************************/
static subpicture_t *Filter( filter_t *p_filter, mtime_t date )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    subpicture_t *p_spu;
    video_format_t fmt;
    subpicture_region_t *p_region;
    int i_feed, i_item;
    rss_feed_t *p_feed;
    memset( &fmt, 0, sizeof(video_format_t) );
    vlc_mutex_lock( &p_sys->lock );
    /* Check if the feeds have been fetched and that we have some feeds */
    /* TODO: check that we have items for each feeds */
    if( !p_sys->b_fetched && p_sys->i_feeds > 0 )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    if( p_sys->last_date
       + ( p_sys->i_cur_char == 0 &&
           p_sys->i_cur_item == ( p_sys->i_title == scroll_title ? -1 : 0 ) ? 5 : 1 )
           /* ( ... ? 5 : 1 ) means "wait 5 times more for the 1st char" */
       * p_sys->i_speed > date )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    p_sys->last_date = date;
    p_sys->i_cur_char++;
    if( p_sys->i_cur_item == -1 ?
            p_sys->p_feeds[p_sys->i_cur_feed].psz_title[p_sys->i_cur_char] == 0 :
            p_sys->p_feeds[p_sys->i_cur_feed].p_items[p_sys->i_cur_item].psz_title[p_sys->i_cur_char] == 0 )
    {
        p_sys->i_cur_char = 0;
        p_sys->i_cur_item++;
        if( p_sys->i_cur_item >= p_sys->p_feeds[p_sys->i_cur_feed].i_items )
        {
            if( p_sys->i_title == scroll_title )
                p_sys->i_cur_item = -1;
            else
                p_sys->i_cur_item = 0;
            p_sys->i_cur_feed = (p_sys->i_cur_feed + 1)%p_sys->i_feeds;
        }
    }
    p_spu = filter_NewSubpicture( p_filter );
    if( !p_spu )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    fmt.i_chroma = VLC_CODEC_TEXT;
    p_spu->p_region = subpicture_region_New( &fmt );
    if( !p_spu->p_region )
    {
        p_filter->pf_sub_buffer_del( p_filter, p_spu );
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    /* Generate the string that will be displayed. This string is supposed to
       be p_sys->i_length characters long. */
    i_item = p_sys->i_cur_item;
    i_feed = p_sys->i_cur_feed;
    p_feed = &p_sys->p_feeds[i_feed];
    if( ( p_feed->p_pic && p_sys->i_title == default_title )
        || p_sys->i_title == hide_title )
    {
        /* Don't display the feed's title if we have an image */
        snprintf( p_sys->psz_marquee, p_sys->i_length, "%s",
                  p_sys->p_feeds[i_feed].p_items[i_item].psz_title
                  +p_sys->i_cur_char );
    }
    else if( ( !p_feed->p_pic && p_sys->i_title == default_title )
             || p_sys->i_title == prepend_title )
    {
        snprintf( p_sys->psz_marquee, p_sys->i_length, "%s : %s",
                  p_sys->p_feeds[i_feed].psz_title,
                  p_sys->p_feeds[i_feed].p_items[i_item].psz_title
                  +p_sys->i_cur_char );
    }
    else /* scrolling title */
    {
        if( i_item == -1 )
            snprintf( p_sys->psz_marquee, p_sys->i_length, "%s : %s",
                      p_sys->p_feeds[i_feed].psz_title + p_sys->i_cur_char,
                      p_sys->p_feeds[i_feed].p_items[i_item+1].psz_title );
        else
            snprintf( p_sys->psz_marquee, p_sys->i_length, "%s",
                      p_sys->p_feeds[i_feed].p_items[i_item].psz_title
                      +p_sys->i_cur_char );
    }
    while( strlen( p_sys->psz_marquee ) < (unsigned int)p_sys->i_length )
    {
        i_item++;
        if( i_item == p_sys->p_feeds[i_feed].i_items ) break;
        snprintf( strchr( p_sys->psz_marquee, 0 ),
                  p_sys->i_length - strlen( p_sys->psz_marquee ),
                  " - %s",
                  p_sys->p_feeds[i_feed].p_items[i_item].psz_title );
    }
    /* Calls to snprintf might split multibyte UTF8 chars ...
     * which freetype doesn't like. */
    {
        char *a = strdup( p_sys->psz_marquee );
        char *a2 = a;
        char *b = p_sys->psz_marquee;
        EnsureUTF8( p_sys->psz_marquee );
        /* we want to use ' ' instead of '?' for erroneous chars */
        while( *b != '\0' )
        {
            if( *b != *a ) *b = ' ';
            b++;a++;
        }
        free( a2 );
    }
    p_spu->p_region->psz_text = strdup(p_sys->psz_marquee);
    if( p_sys->p_style->i_font_size > 0 )
        p_spu->p_region->fmt.i_visible_height = p_sys->p_style->i_font_size;
    p_spu->i_start = date;
    p_spu->i_stop  = 0;
    p_spu->b_ephemer = true;
    /*  where to locate the string: */
    if( p_sys->i_pos < 0 )
    {   /*  set to an absolute xy */
        p_spu->p_region->i_align = SUBPICTURE_ALIGN_LEFT | SUBPICTURE_ALIGN_TOP;
        p_spu->b_absolute = true;
    }
    else
    {   /* set to one of the 9 relative locations */
        p_spu->p_region->i_align = p_sys->i_pos;
        p_spu->b_absolute = false;
    }
    p_spu->p_region->i_x = p_sys->i_xoff;
    p_spu->p_region->i_y = p_sys->i_yoff;
    p_spu->p_region->p_style = text_style_Duplicate( p_sys->p_style );
    if( p_feed->p_pic )
    {
        /* Display the feed's image */
        picture_t *p_pic = p_feed->p_pic;
        video_format_t fmt_out;
        memset( &fmt_out, 0, sizeof(video_format_t) );
        fmt_out.i_chroma = VLC_CODEC_YUVA;
        fmt_out.i_sar_num = fmt_out.i_sar_den = 1;
        fmt_out.i_width =
            fmt_out.i_visible_width = p_pic->p[Y_PLANE].i_visible_pitch;
        fmt_out.i_height =
            fmt_out.i_visible_height = p_pic->p[Y_PLANE].i_visible_lines;
        p_region = subpicture_region_New( &fmt_out );
        if( !p_region )
        {
            msg_Err( p_filter, "cannot allocate SPU region" );
        }
        else
        {
            p_region->i_x = p_spu->p_region->i_x;
            p_region->i_y = p_spu->p_region->i_y;
            /* FIXME the copy is probably not needed anymore */
            picture_Copy( p_region->p_picture, p_pic );
            p_spu->p_region->p_next = p_region;
            /* Offset text to display right next to the image */
            p_spu->p_region->i_x += fmt_out.i_visible_width;
        }
    }
    vlc_mutex_unlock( &p_sys->lock );
    return p_spu;
}
