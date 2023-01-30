 *****************************************************************************/
void puzzle_drw_adv_pce_in_plane( filter_t *p_filter, picture_t *p_pic_in, picture_t *p_pic_out, uint8_t i_plane, piece_t *ps_piece)
{
    /* here we still have rectangular shape but angle is not 0 */
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
    for (int32_t i_y = 0; i_y < i_lines; i_y++) {
        int32_t i_current_src_y = i_pic_start_y + i_y;
        if ( ( i_current_src_y >= 0 ) && ( i_current_src_y < i_src_visible_lines ) ) {
            for (int32_t i_x = 0; i_x < i_width; i_x++) {
                int32_t i_current_dst_x = i_desk_start_x + i_x * ps_piece->i_step_x_x + i_y * ps_piece->i_step_y_x;
                int32_t i_current_dst_y = i_desk_start_y + i_x * ps_piece->i_step_x_y + i_y * ps_piece->i_step_y_y;
                int32_t i_current_src_x = i_pic_start_x + i_x;
                if ( ( i_current_dst_x  >= 0 ) && ( i_current_src_x >= 0 )
                        && ( i_current_dst_x  < i_dst_width ) && ( i_current_src_x < i_src_width )
                        && ( i_current_dst_y >= 0 ) && ( i_current_dst_y < i_dst_visible_lines ) )
                {
                    memcpy( p_dst + i_current_dst_y * i_dst_pitch + i_current_dst_x * i_pixel_pitch,
                            p_src + i_current_src_y * i_src_pitch + i_current_src_x * i_pixel_pitch,
                           i_pixel_pitch );
                }
            }
        }
    }
    return;
}
