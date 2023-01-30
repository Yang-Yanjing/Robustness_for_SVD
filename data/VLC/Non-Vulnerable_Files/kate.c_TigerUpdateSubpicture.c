   common case. */
static void TigerUpdateSubpicture( subpicture_t *p_subpic,
                                   const video_format_t *p_fmt_src,
                                   const video_format_t *p_fmt_dst,
                                   mtime_t ts )
{
    decoder_sys_t *p_sys = p_subpic->updater.p_sys->p_dec_sys;
    plane_t *p_plane;
    kate_float t;
    int i_ret;
    /* time in seconds from the start of the stream */
    t = (p_subpic->updater.p_sys->i_start + ts - p_subpic->i_start ) / 1000000.0f;
    PROFILE_START( TigerUpdateSubpicture );
    /* create a full frame region - this will also tell Tiger the size of the frame */
    video_format_t fmt = *p_fmt_dst;
    fmt.i_chroma         = VLC_CODEC_RGBA;
    fmt.i_bits_per_pixel = 0;
    fmt.i_width          =
    fmt.i_visible_width  = p_fmt_src->i_width;
    fmt.i_height         =
    fmt.i_visible_height = p_fmt_src->i_height;
    fmt.i_x_offset       = fmt.i_y_offset = 0;
    subpicture_region_t *p_r = subpicture_region_New( &fmt );
    if( !p_r )
        return;
    p_r->i_x = 0;
    p_r->i_y = 0;
    p_r->i_align = SUBPICTURE_ALIGN_TOP | SUBPICTURE_ALIGN_LEFT;
    vlc_mutex_lock( &p_sys->lock );
    p_plane = &p_r->p_picture->p[0];
    i_ret = tiger_renderer_set_buffer( p_sys->p_tr, p_plane->p_pixels, fmt.i_width, p_plane->i_lines, p_plane->i_pitch, 1 );
    if( i_ret < 0 )
    {
        goto failure;
    }
    PROFILE_START( tiger_renderer_update );
    i_ret = tiger_renderer_update( p_sys->p_tr, t, 1 );
    if( i_ret < 0 )
    {
        goto failure;
    }
    PROFILE_STOP( tiger_renderer_update );
    PROFILE_START( tiger_renderer_render );
    i_ret = tiger_renderer_render( p_sys->p_tr );
    if( i_ret < 0 )
    {
        goto failure;
    }
    PROFILE_STOP( tiger_renderer_render );
    PostprocessTigerImage( p_plane, fmt.i_width );
    p_subpic->p_region = p_r;
    p_sys->b_dirty = false;
    PROFILE_STOP( TigerUpdateSubpicture );
    vlc_mutex_unlock( &p_sys->lock );
    return;
failure:
    vlc_mutex_unlock( &p_sys->lock );
    subpicture_region_ChainDelete( p_r );
}
