 *****************************************************************************/
void puzzle_draw_sign(picture_t *p_pic_out, int32_t i_x, int32_t i_y, int32_t i_width, int32_t i_lines, const char **ppsz_sign, bool b_reverse)
{
    plane_t *p_out = &p_pic_out->p[Y_PLANE];
    int32_t i_pixel_pitch    = p_pic_out->p[Y_PLANE].i_pixel_pitch;
    uint8_t i_Y;
    i_Y = ( p_out->p_pixels[ i_y * p_out->i_pitch + i_x ] >= 0x7F ) ? 0x00 : 0xFF;
    for( int32_t y = 0; y < i_lines ; y++ )
        for( int32_t x = 0; x < i_width; x++ ) {
            int32_t i_dst_x = ( x + i_x ) * i_pixel_pitch;
            int32_t i_dst_y = y + i_y;
            if ( ppsz_sign[y][b_reverse?i_width-1-x:x] == 'o' ) {
                if ((i_dst_x < p_out->i_visible_pitch) && (i_dst_y < p_out->i_visible_lines) && (i_dst_x >= 0 ) && (i_dst_y >= 0))
                    memset( &p_out->p_pixels[ i_dst_y * p_out->i_pitch + i_dst_x ],   i_Y,  p_out->i_pixel_pitch );
            }
            else if ( ppsz_sign[y][b_reverse?i_width-1-x:x] == '.' ) {
                if ((i_dst_x < p_out->i_visible_pitch) && (i_dst_y < p_out->i_visible_lines) && (i_dst_x >= 0 ) && (i_dst_y >= 0))
                    p_out->p_pixels[ i_dst_y * p_out->i_pitch + i_dst_x ] = p_out->p_pixels[ i_dst_y * p_out->i_pitch + i_dst_x ] / 2 + i_Y / 2;
            }
        }
}
