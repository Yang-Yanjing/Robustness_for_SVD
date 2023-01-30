 */
static picture_t *FilterVideo( filter_t *p_filter, picture_t *p_src )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    logo_list_t *p_list = &p_sys->list;
    picture_t *p_dst = filter_NewPicture( p_filter );
    if( !p_dst )
        goto exit;
    picture_Copy( p_dst, p_src );
    /* */
    vlc_mutex_lock( &p_sys->lock );
    logo_t *p_logo;
    if( p_list->i_next_pic < p_src->date )
        p_logo = LogoListNext( p_list, p_src->date );
    else
        p_logo = LogoListCurrent( p_list );
    /* */
    const picture_t *p_pic = p_logo->p_pic;
    if( p_pic )
    {
        const video_format_t *p_fmt = &p_pic->format;
        const int i_dst_w = p_filter->fmt_out.video.i_visible_width;
        const int i_dst_h = p_filter->fmt_out.video.i_visible_height;
        if( p_sys->i_pos )
        {
            if( p_sys->i_pos & SUBPICTURE_ALIGN_BOTTOM )
            {
                p_sys->i_pos_y = i_dst_h - p_fmt->i_visible_height;
            }
            else if ( !(p_sys->i_pos & SUBPICTURE_ALIGN_TOP) )
            {
                p_sys->i_pos_y = ( i_dst_h - p_fmt->i_visible_height ) / 2;
            }
            else
            {
                p_sys->i_pos_y = 0;
            }
            if( p_sys->i_pos & SUBPICTURE_ALIGN_RIGHT )
            {
                p_sys->i_pos_x = i_dst_w - p_fmt->i_visible_width;
            }
            else if ( !(p_sys->i_pos & SUBPICTURE_ALIGN_LEFT) )
            {
                p_sys->i_pos_x = ( i_dst_w - p_fmt->i_visible_width ) / 2;
            }
            else
            {
                p_sys->i_pos_x = 0;
            }
        }
        if( p_sys->i_pos_x < 0 || p_sys->i_pos_y < 0 )
        {
            msg_Warn( p_filter,
                "logo(%ix%i) doesn't fit into video(%ix%i)",
                p_fmt->i_visible_width, p_fmt->i_visible_height,
                i_dst_w,i_dst_h );
            p_sys->i_pos_x = (p_sys->i_pos_x > 0) ? p_sys->i_pos_x : 0;
            p_sys->i_pos_y = (p_sys->i_pos_y > 0) ? p_sys->i_pos_y : 0;
        }
        /* */
        const int i_alpha = p_logo->i_alpha != -1 ? p_logo->i_alpha : p_list->i_alpha;
        if( filter_ConfigureBlend( p_sys->p_blend, i_dst_w, i_dst_h, p_fmt ) ||
            filter_Blend( p_sys->p_blend, p_dst, p_sys->i_pos_x, p_sys->i_pos_y,
                          p_pic, i_alpha ) )
        {
            msg_Err( p_filter, "failed to blend a picture" );
        }
    }
    vlc_mutex_unlock( &p_sys->lock );
exit:
    picture_Release( p_src );
    return p_dst;
}
