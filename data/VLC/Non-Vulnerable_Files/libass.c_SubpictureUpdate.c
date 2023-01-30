}
static void SubpictureUpdate( subpicture_t *p_subpic,
                              const video_format_t *p_fmt_src,
                              const video_format_t *p_fmt_dst,
                              mtime_t i_ts )
{
    VLC_UNUSED( p_fmt_src ); VLC_UNUSED( p_fmt_dst ); VLC_UNUSED( i_ts );
    decoder_sys_t *p_sys = p_subpic->updater.p_sys->p_dec_sys;
    video_format_t fmt = p_sys->fmt;
    ASS_Image *p_img = p_subpic->updater.p_sys->p_img;
    /* */
    p_subpic->i_original_picture_height = fmt.i_visible_height;
    p_subpic->i_original_picture_width = fmt.i_visible_width;
    /* XXX to improve efficiency we merge regions that are close minimizing
     * the lost surface.
     * libass tends to create a lot of small regions and thus spu engine
     * reinstanciate a lot the scaler, and as we do not support subpel blending
     * it looks ugly (text unaligned).
     */
    const int i_max_region = 4;
    rectangle_t region[i_max_region];
    const int i_region = BuildRegions( region, i_max_region, p_img, fmt.i_width, fmt.i_height );
    if( i_region <= 0 )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return;
    }
    /* Allocate the regions and draw them */
    subpicture_region_t **pp_region_last = &p_subpic->p_region;
    for( int i = 0; i < i_region; i++ )
    {
        subpicture_region_t *r;
        video_format_t fmt_region;
        /* */
        fmt_region = fmt;
        fmt_region.i_width =
        fmt_region.i_visible_width  = region[i].x1 - region[i].x0;
        fmt_region.i_height =
        fmt_region.i_visible_height = region[i].y1 - region[i].y0;
        r = subpicture_region_New( &fmt_region );
        if( !r )
            break;
        r->i_x = region[i].x0;
        r->i_y = region[i].y0;
        r->i_align = SUBPICTURE_ALIGN_TOP | SUBPICTURE_ALIGN_LEFT;
        /* */
        RegionDraw( r, p_img );
        /* */
        *pp_region_last = r;
        pp_region_last = &r->p_next;
    }
    vlc_mutex_unlock( &p_sys->lock );
}
