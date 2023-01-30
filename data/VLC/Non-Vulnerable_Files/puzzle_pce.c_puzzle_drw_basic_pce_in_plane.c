 *****************************************************************************/
void puzzle_drw_basic_pce_in_plane( filter_t *p_filter, picture_t *p_pic_in, picture_t *p_pic_out, uint8_t i_plane, piece_t *ps_piece)
{
    /* basic version rectangular & angle = 0 */
    filter_sys_t *p_sys = p_filter->p_sys;
    if ((p_sys->ps_puzzle_array == NULL) || (p_sys->ps_pieces == NULL) || (ps_piece == NULL))
        return;
    const int32_t i_src_pitch    = p_pic_in->p[i_plane].i_pitch;
    const int32_t i_dst_pitch    = p_pic_out->p[i_plane].i_pitch;
    const int32_t i_src_width    = p_pic_in->p[i_plane].i_pitch / p_pic_in->p[i_plane].i_pixel_pitch;
    const int32_t i_dst_width    = p_pic_out->p[i_plane].i_pitch / p_pic_out->p[i_plane].i_pixel_pitch;
    const int32_t i_pixel_pitch  = p_pic_out->p[i_plane].i_pixel_pitch;
    const int32_t i_src_visible_lines    = p_pic_in->p[i_plane].i_visible_lines;
    const int32_t i_dst_visible_lines    = p_pic_out->p[i_plane].i_visible_lines;
    uint8_t *p_src = p_pic_in->p[i_plane].p_pixels;
    uint8_t *p_dst = p_pic_out->p[i_plane].p_pixels;
    const int32_t i_desk_start_x = ps_piece->ps_piece_in_plane[i_plane].i_actual_x;
    const int32_t i_desk_start_y = ps_piece->ps_piece_in_plane[i_plane].i_actual_y;
    const int32_t i_pic_start_x = ps_piece->ps_piece_in_plane[i_plane].i_original_x;
    const int32_t i_pic_start_y = ps_piece->ps_piece_in_plane[i_plane].i_original_y;
    const int32_t i_width = ps_piece->ps_piece_in_plane[i_plane].i_width;
    const int32_t i_lines = ps_piece->ps_piece_in_plane[i_plane].i_lines;
    const int32_t i_ofs_x   =           __MAX(0, __MAX(-i_desk_start_x,-i_pic_start_x));
    const int32_t i_count_x = i_width - __MAX(0, __MAX(i_desk_start_x + i_width - i_dst_width, i_pic_start_x + i_width - i_src_width ));
    const int32_t i_ofs_y   =           __MAX(0, __MAX(-i_desk_start_y,-i_pic_start_y));
    const int32_t i_count_y = i_lines - __MAX(0, __MAX(i_desk_start_y + i_lines - i_dst_visible_lines, i_pic_start_y + i_lines - i_src_visible_lines ));
    for (int32_t i_y = i_ofs_y; i_y < i_count_y; i_y++) {
        memcpy( p_dst + (i_desk_start_y + i_y) * i_dst_pitch + ( i_desk_start_x + i_ofs_x ) * i_pixel_pitch,
            p_src + (i_pic_start_y + i_y) * i_src_pitch + ( i_pic_start_x + i_ofs_x ) * i_pixel_pitch,
            ( i_count_x - i_ofs_x ) * i_pixel_pitch );
    }
    return;
}
