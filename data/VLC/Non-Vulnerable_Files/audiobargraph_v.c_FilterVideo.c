 */
static picture_t *FilterVideo(filter_t *p_filter, picture_t *p_src)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    BarGraph_t *p_BarGraph = &(p_sys->p_BarGraph);
    picture_t *p_dst = filter_NewPicture(p_filter);
    if (!p_dst) {
        picture_Release(p_src);
        return NULL;
    }
    picture_Copy(p_dst, p_src);
    /* */
    vlc_mutex_lock(&p_sys->lock);
    /* */
    const picture_t *p_pic = p_BarGraph->p_pic;
    if (!p_pic)
        goto out;
    const video_format_t *p_fmt = &p_pic->format;
    const int i_dst_w = p_filter->fmt_out.video.i_visible_width;
    const int i_dst_h = p_filter->fmt_out.video.i_visible_height;
    if (p_sys->i_pos) {
        if (p_sys->i_pos & SUBPICTURE_ALIGN_BOTTOM)
            p_sys->i_pos_y = i_dst_h - p_fmt->i_visible_height;
        else if (!(p_sys->i_pos & SUBPICTURE_ALIGN_TOP))
            p_sys->i_pos_y = (i_dst_h - p_fmt->i_visible_height) / 2;
        else
            p_sys->i_pos_y = 0;
        if (p_sys->i_pos & SUBPICTURE_ALIGN_RIGHT)
            p_sys->i_pos_x = i_dst_w - p_fmt->i_visible_width;
        else if (!(p_sys->i_pos & SUBPICTURE_ALIGN_LEFT))
            p_sys->i_pos_x = (i_dst_w - p_fmt->i_visible_width) / 2;
        else
            p_sys->i_pos_x = 0;
    }
    /* */
    const int i_alpha = p_BarGraph->i_alpha;
    if (filter_ConfigureBlend(p_sys->p_blend, i_dst_w, i_dst_h, p_fmt) ||
            filter_Blend(p_sys->p_blend, p_dst, p_sys->i_pos_x, p_sys->i_pos_y,
                p_pic, i_alpha))
        msg_Err(p_filter, "failed to blend a picture");
out:
    vlc_mutex_unlock(&p_sys->lock);
    picture_Release(p_src);
    return p_dst;
}
