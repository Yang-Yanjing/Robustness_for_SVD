}
void video_format_CopyCrop( video_format_t *p_dst, const video_format_t *p_src )
{
    p_dst->i_x_offset       = p_src->i_x_offset;
    p_dst->i_y_offset       = p_src->i_y_offset;
    p_dst->i_visible_width  = p_src->i_visible_width;
    p_dst->i_visible_height = p_src->i_visible_height;
}
