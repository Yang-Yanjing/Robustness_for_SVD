 *****************************************************************************/
void puzzle_draw_borders( filter_t *p_filter, picture_t *p_pic_in, picture_t *p_pic_out)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    for( uint8_t i_plane = 0; i_plane < p_pic_out->i_planes; i_plane++ ) {
        const int32_t i_in_pitch      = p_sys->ps_pict_planes[i_plane].i_pitch;
        const int32_t i_out_pitch     = p_sys->ps_desk_planes[i_plane].i_pitch;
        const int32_t i_lines         = p_sys->ps_desk_planes[i_plane].i_lines;
        const int32_t i_visible_pitch = p_sys->ps_desk_planes[i_plane].i_visible_pitch;
        const int32_t i_border_pitch  = p_sys->ps_desk_planes[i_plane].i_border_width * p_sys->ps_desk_planes[i_plane].i_pixel_pitch;
        const int32_t i_border_lines  = p_sys->ps_desk_planes[i_plane].i_border_lines;
        uint8_t *p_src = p_pic_in->p[i_plane].p_pixels;
        uint8_t *p_dst = p_pic_out->p[i_plane].p_pixels;
        for (int32_t y = 0 ; y < i_border_lines; y++)
            memcpy( &p_dst[y * i_out_pitch], &p_src[y * i_in_pitch], i_visible_pitch);
        for (int32_t y = i_lines - i_border_lines ; y < i_lines; y++)
            memcpy( &p_dst[y * i_out_pitch], &p_src[y * i_in_pitch], i_visible_pitch);
        for (int32_t y = i_border_lines ; y < i_lines - i_border_lines; y++) {
            memcpy( &p_dst[y * i_out_pitch], &p_src[y * i_in_pitch], i_border_pitch);
            memcpy( &p_dst[y * i_out_pitch + i_visible_pitch - i_border_pitch], &p_src[y * i_in_pitch + i_visible_pitch - i_border_pitch], i_border_pitch);
        }
    }
}
