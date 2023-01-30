}
void video_format_ScaleCropAr( video_format_t *p_dst, const video_format_t *p_src )
{
    p_dst->i_x_offset       = (uint64_t)p_src->i_x_offset       * p_dst->i_width  / p_src->i_width;
    p_dst->i_y_offset       = (uint64_t)p_src->i_y_offset       * p_dst->i_height / p_src->i_height;
    p_dst->i_visible_width  = (uint64_t)p_src->i_visible_width  * p_dst->i_width  / p_src->i_width;
    p_dst->i_visible_height = (uint64_t)p_src->i_visible_height * p_dst->i_height / p_src->i_height;
    p_dst->i_sar_num *= p_src->i_width;
    p_dst->i_sar_den *= p_dst->i_width;
    vlc_ureduce(&p_dst->i_sar_num, &p_dst->i_sar_den,
                p_dst->i_sar_num, p_dst->i_sar_den, 65536);
    p_dst->i_sar_num *= p_dst->i_height;
    p_dst->i_sar_den *= p_src->i_height;
    vlc_ureduce(&p_dst->i_sar_num, &p_dst->i_sar_den,
                p_dst->i_sar_num, p_dst->i_sar_den, 65536);
}
