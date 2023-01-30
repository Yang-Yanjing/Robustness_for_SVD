}
static inline bool copy_rect( filter_sys_t* p_sys,
                              uint16_t i_x, uint16_t i_y,
                              uint16_t i_w, uint16_t i_h,
                              uint16_t i_sx, uint16_t i_sy )
{
    plane_t *p_Y = p_sys->p_pic->p+Y_PLANE;
    plane_t *p_U = p_sys->p_pic->p+U_PLANE;
    plane_t *p_V = p_sys->p_pic->p+V_PLANE;
    plane_t *p_A = p_sys->p_pic->p+A_PLANE;
    int i_pitch = p_Y->i_pitch;
    int i_lines = p_Y->i_lines;
    fprintf( stderr, "copy_rect: (%d,%d)+(%d,%d) -> (%d,%d)\n", i_x, i_y, i_w, i_h, i_sx, i_sy );
    if( i_x + i_w > i_pitch || i_sx + i_w > i_pitch )
        return false;
    if( i_y + i_h > i_lines || i_sy + i_h > i_lines)
        return false;
    if( i_w <= 0 || i_h <= 0 )
        return true;
    uint8_t *pb_buffer = calloc( i_w * i_h, 4 );
    if( !pb_buffer )
        return false;
    for( int i_line = 0; i_line < i_h; i_line++ )
    {
        for( int i_column = 0; i_column < i_w; i_column++ )
        {
            const int i_src_offset = ( i_sy + i_line ) * i_pitch + i_sx + i_column;
            const int i_tmp_offset = (    0 + i_line ) *     i_w +    0 + i_column;
            pb_buffer[4*i_tmp_offset + 0] = p_Y->p_pixels[i_src_offset];
            pb_buffer[4*i_tmp_offset + 1] = p_U->p_pixels[i_src_offset];
            pb_buffer[4*i_tmp_offset + 2] = p_V->p_pixels[i_src_offset];
            pb_buffer[4*i_tmp_offset + 3] = p_A->p_pixels[i_src_offset];
        }
    }
    for( int i_line = 0; i_line < i_h; i_line++ )
    {
        for( int i_column = 0; i_column < i_w; i_column++ )
        {
            const int i_tmp_offset = (   0 + i_line ) *     i_w +   0 + i_column;
            const int i_dst_offset = ( i_y + i_line ) * i_pitch + i_x + i_column;
            p_Y->p_pixels[i_dst_offset] = pb_buffer[4*i_tmp_offset + 0];
            p_U->p_pixels[i_dst_offset] = pb_buffer[4*i_tmp_offset + 1];
            p_V->p_pixels[i_dst_offset] = pb_buffer[4*i_tmp_offset + 2];
            p_A->p_pixels[i_dst_offset] = pb_buffer[4*i_tmp_offset + 3];
        }
    }
    free( pb_buffer );
    return true;
}
