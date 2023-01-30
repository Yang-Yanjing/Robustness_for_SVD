 ****************************************************************************/
static int SubpictureValidate( subpicture_t *p_subpic,
                               bool b_fmt_src, const video_format_t *p_fmt_src,
                               bool b_fmt_dst, const video_format_t *p_fmt_dst,
                               mtime_t i_ts )
{
    decoder_sys_t *p_sys = p_subpic->updater.p_sys->p_dec_sys;
    vlc_mutex_lock( &p_sys->lock );
    video_format_t fmt = *p_fmt_dst;
    fmt.i_chroma         = VLC_CODEC_RGBA;
    fmt.i_bits_per_pixel = 0;
    fmt.i_x_offset       = 0;
    fmt.i_y_offset       = 0;
    if( b_fmt_src || b_fmt_dst )
    {
        ass_set_frame_size( p_sys->p_renderer, fmt.i_visible_width, fmt.i_visible_height );
        const double src_ratio = (double)p_fmt_src->i_visible_width / p_fmt_src->i_visible_height;
        const double dst_ratio = (double)p_fmt_dst->i_visible_width / p_fmt_dst->i_visible_height;
        ass_set_aspect_ratio( p_sys->p_renderer, dst_ratio / src_ratio, 1 );
        p_sys->fmt = fmt;
    }
    /* */
    const mtime_t i_stream_date = p_subpic->updater.p_sys->i_pts + (i_ts - p_subpic->i_start);
    int i_changed;
    ASS_Image *p_img = ass_render_frame( p_sys->p_renderer, p_sys->p_track,
                                         i_stream_date/1000, &i_changed );
    if( !i_changed && !b_fmt_src && !b_fmt_dst &&
        (p_img != NULL) == (p_subpic->p_region != NULL) )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return VLC_SUCCESS;
    }
    p_subpic->updater.p_sys->p_img = p_img;
    /* The lock is released by SubpictureUpdate */
    return VLC_EGENERIC;
}
