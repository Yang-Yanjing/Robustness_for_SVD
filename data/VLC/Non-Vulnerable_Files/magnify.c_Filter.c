 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_t *p_outpic;
    int v_w, v_h;
    picture_t *p_converted;
    plane_t *p_oyp;
    int i_plane;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* */
    const bool b_visible = p_sys->b_visible;
    const int o_x = p_sys->i_x;
    const int o_y = p_sys->i_y;
    const int o_zoom = p_sys->i_zoom;
    /* background magnified image */
    if( o_zoom != ZOOM_FACTOR )
    {
        video_format_t fmt_in;
        video_format_t fmt_out;
        picture_t crop;
        crop = *p_pic;
        for( i_plane = 0; i_plane < p_pic->i_planes; i_plane++ )
        {
            const int o_yp = o_y * p_outpic->p[i_plane].i_lines / p_outpic->p[Y_PLANE].i_lines;
            const int o_xp = o_x * p_outpic->p[i_plane].i_pitch / p_outpic->p[Y_PLANE].i_pitch;
            crop.p[i_plane].p_pixels += o_yp * p_pic->p[i_plane].i_pitch + o_xp;
        }
        /* */
        fmt_in = p_filter->fmt_in.video;
        fmt_in.i_width  = fmt_in.i_visible_width  = (fmt_in.i_width  * ZOOM_FACTOR / o_zoom) & ~1;
        fmt_in.i_height = fmt_in.i_visible_height = (fmt_in.i_height * ZOOM_FACTOR / o_zoom) & ~1;
        /* */
        fmt_out = p_filter->fmt_out.video;
        p_converted = image_Convert( p_sys->p_image, &crop, &fmt_in, &fmt_out );
        picture_CopyPixels( p_outpic, p_converted );
        picture_Release( p_converted );
    }
    else
    {
        picture_CopyPixels( p_outpic, p_pic );
    }
    /* */
    p_oyp = &p_outpic->p[Y_PLANE];
    if( b_visible )
    {
        video_format_t fmt_out;
        /* image visualization */
        fmt_out = p_filter->fmt_out.video;
        fmt_out.i_width  = fmt_out.i_visible_width  = (fmt_out.i_width /VIS_ZOOM) & ~1;
        fmt_out.i_height = fmt_out.i_visible_height = (fmt_out.i_height/VIS_ZOOM) & ~1;
        p_converted = image_Convert( p_sys->p_image, p_pic,
                                     &p_pic->format, &fmt_out );
        /* It will put only what can be copied at the top left */
        picture_CopyPixels( p_outpic, p_converted );
        picture_Release( p_converted );
        /* white rectangle on visualization */
        v_w = __MIN( fmt_out.i_width  * ZOOM_FACTOR / o_zoom, fmt_out.i_width - 1 );
        v_h = __MIN( fmt_out.i_height * ZOOM_FACTOR / o_zoom, fmt_out.i_height - 1 );
        DrawRectangle( p_oyp->p_pixels, p_oyp->i_pitch,
                       p_oyp->i_pitch, p_oyp->i_lines,
                       o_x/VIS_ZOOM, o_y/VIS_ZOOM,
                       v_w, v_h );
        /* */
        v_h = fmt_out.i_height + 1;
    }
    else
    {
        v_h = 1;
    }
    /* print a small "VLC ZOOM" */
    if( b_visible || p_sys->i_last_activity + p_sys->i_hide_timeout > mdate() )
        DrawZoomStatus( p_oyp->p_pixels, p_oyp->i_pitch, p_oyp->i_pitch, p_oyp->i_lines,
                        1, v_h, b_visible );
    if( b_visible )
    {
        /* zoom gauge */
        memset( p_oyp->p_pixels + (v_h+9)*p_oyp->i_pitch, 0xff, 41 );
        for( int y = v_h + 10; y < v_h + 90; y++ )
        {
            int i_width = v_h + 90 - y;
            i_width = i_width * i_width / 160;
            if( (80 - y + v_h)*ZOOM_FACTOR/10 < o_zoom )
            {
                memset( p_oyp->p_pixels + y*p_oyp->i_pitch, 0xff, i_width );
            }
            else
            {
                p_oyp->p_pixels[y*p_oyp->i_pitch] = 0xff;
                p_oyp->p_pixels[y*p_oyp->i_pitch + i_width - 1] = 0xff;
            }
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
