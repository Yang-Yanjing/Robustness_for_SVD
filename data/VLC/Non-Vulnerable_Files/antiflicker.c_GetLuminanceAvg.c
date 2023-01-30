 *****************************************************************************/
static int GetLuminanceAvg( picture_t *p_pic )
{
    uint8_t *p_yplane_out = p_pic->p[Y_PLANE].p_pixels;
    int i_num_lines = p_pic->p[Y_PLANE].i_visible_lines;
    int i_num_cols = p_pic->p[Y_PLANE].i_visible_pitch;
    int i_in_pitch = p_pic->p[Y_PLANE].i_pitch;
    if( i_num_lines == 0 || i_num_cols == 0 )
        return 0;
    unsigned lum_sum = 0;
    for( int i_line = 0 ; i_line < i_num_lines ; ++i_line )
    {
        for( int i_col = 0 ; i_col < i_num_cols; ++i_col )
        {
            lum_sum += p_yplane_out[i_line*i_in_pitch+i_col];
        }
    }
    unsigned div = i_num_lines * i_num_cols;
    return (lum_sum + (div>>1)) / div;
}
