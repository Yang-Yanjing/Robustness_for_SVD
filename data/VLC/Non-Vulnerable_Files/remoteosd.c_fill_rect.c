}
static inline bool fill_rect( filter_sys_t* p_sys,
                              uint16_t i_x, uint16_t i_y,
                              uint16_t i_w, uint16_t i_h,
                              uint8_t i_color)
{
    plane_t *p_outY = p_sys->p_pic->p+Y_PLANE;
    plane_t *p_outU = p_sys->p_pic->p+U_PLANE;
    plane_t *p_outV = p_sys->p_pic->p+V_PLANE;
    plane_t *p_outA = p_sys->p_pic->p+A_PLANE;
    int i_pitch = p_outY->i_pitch;
    int i_lines = p_outY->i_lines;
    if ( i_x + i_w > i_pitch)
        return false;
    if ( i_y + i_h > i_lines)
        return false;
    int i_line_offset = i_y * i_pitch;
    uint8_t i_yuv_y = p_sys->ar_color_table_yuv[i_color][0];
    uint8_t i_yuv_u = p_sys->ar_color_table_yuv[i_color][1];
    uint8_t i_yuv_v = p_sys->ar_color_table_yuv[i_color][2];
    uint8_t i_alpha = p_sys->ar_color_table_yuv[i_color][3];
    for( int i_line = 0; i_line < i_h; i_line++ )
    {
        for( int i_column = 0; i_column < i_w; i_column++ )
        {
            int i_total_offset = i_line_offset + i_x + i_column;
            p_outY->p_pixels[ i_total_offset ] = i_yuv_y;
            p_outU->p_pixels[ i_total_offset ] = i_yuv_u;
            p_outV->p_pixels[ i_total_offset ] = i_yuv_v;
            p_outA->p_pixels[ i_total_offset ] = i_alpha;
        }
        i_line_offset += i_pitch;
    }
    return true;
}
