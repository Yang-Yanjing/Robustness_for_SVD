 *****************************************************************************/
static void YUV422VerticalMirror( picture_t *p_pic, picture_t *p_outpic,
                                 int i_plane, bool b_left_to_right,
                                 bool b_y_is_odd )
{
    uint8_t *p_in, *p_in_end, *p_line_start, *p_line_end, *p_out;
    p_in = p_pic->p[i_plane].p_pixels;
    p_in_end = p_in + p_pic->p[i_plane].i_visible_lines
        * p_pic->p[i_plane].i_pitch;
    p_out = p_outpic->p[i_plane].p_pixels;
    while( p_in < p_in_end )
    {
        p_line_start = p_in;
        p_line_end = p_in + p_pic->p[i_plane].i_visible_pitch;
        while( p_in < p_line_end )
        {
            /* are we in the left part of the line */
            if ( p_in < p_line_start + ( p_line_end - p_line_start ) / 2 )
            {
                if ( b_left_to_right )
                {
                    *p_out++ = *p_in++;
                    *p_out++ = *p_in++;
                    *p_out++ = *p_in++;
                    *p_out++ = *p_in++;
                }
                else
                {
                    uint8_t *p_start = p_line_end - ( p_in - p_line_start );
                    YUV422Mirror2Pixels( p_out, p_start, b_y_is_odd );
                    p_in += 4;
                    p_out += 4;
                }
            }
            else
            {
                if ( b_left_to_right )
                {
                    uint8_t *p_start = p_line_end - ( p_in - p_line_start );
                    YUV422Mirror2Pixels( p_out, p_start, b_y_is_odd );
                    p_in += 4;
                    p_out += 4;
                }
                else
                {
                    *p_out++ = *p_in++;
                    *p_out++ = *p_in++;
                    *p_out++ = *p_in++;
                    *p_out++ = *p_in++;
                }
            }
        }
        p_in += p_pic->p[i_plane].i_pitch - p_pic->p[i_plane].i_visible_pitch;
        p_out += p_outpic->p[i_plane].i_pitch
            - p_outpic->p[i_plane].i_visible_pitch;
    }
}
