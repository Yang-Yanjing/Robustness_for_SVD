 *****************************************************************************/
void puzzle_fill_rectangle(picture_t *p_pic_out, int32_t i_x, int32_t i_y, int32_t i_w, int32_t i_h, uint8_t i_Y, uint8_t i_U, uint8_t i_V )
{
    uint8_t i_c;
    for( uint8_t i_plane = 0; i_plane < p_pic_out->i_planes; i_plane++ ) {
        plane_t *p_oyp = &p_pic_out->p[i_plane];
        int32_t i_pixel_pitch    = p_pic_out->p[i_plane].i_pixel_pitch;
        if (i_plane == Y_PLANE)
            i_c = i_Y;
        else if (i_plane == U_PLANE)
            i_c = i_U;
        else if (i_plane == V_PLANE)
            i_c = i_V;
        int32_t i_x_min = (     i_x     * p_oyp->i_visible_pitch / p_pic_out->p[0].i_visible_pitch ) * i_pixel_pitch;
        int32_t i_x_max = ( (i_x + i_w) * p_oyp->i_visible_pitch / p_pic_out->p[0].i_visible_pitch ) * i_pixel_pitch;
        int32_t i_y_min =       i_y     * p_oyp->i_visible_lines / p_pic_out->p[0].i_visible_lines;
        int32_t i_y_max =   (i_y + i_h) * p_oyp->i_visible_lines / p_pic_out->p[0].i_visible_lines;
        for( int32_t i_dy = 0; i_dy < i_y_max - i_y_min; i_dy++ )
            memset( &p_oyp->p_pixels[(i_y_min + i_dy) * p_oyp->i_pitch + i_x_min], i_c,  i_x_max - i_x_min);
    }
}
