 *****************************************************************************/
static void RV32VerticalMirror( picture_t *p_pic, picture_t *p_outpic,
                                 int i_plane, bool b_left_to_right )
{
    uint8_t *p_in, *p_in_end, *p_out;
    p_in = p_pic->p[i_plane].p_pixels;
    p_in_end = p_in + p_pic->p[i_plane].i_visible_lines
        * p_pic->p[i_plane].i_pitch;
    p_out = p_outpic->p[i_plane].p_pixels;
    while( p_in < p_in_end )
    {
        uint32_t *p_in32, *p_out32, *p_line_start32, *p_line_end32;
        p_in32 = (uint32_t*) p_in;
        p_out32 = (uint32_t*) p_out;
        p_line_start32 = p_in32;
        p_line_end32 = (uint32_t*) ( p_in + p_pic->p[i_plane].i_visible_pitch) ;
        while( p_in32 < p_line_end32 )
        {
            /* are we in the left part of the line */
            if ( p_in32 < p_line_start32 + ( p_line_end32 - p_line_start32 ) / 2 )
            {
                if ( b_left_to_right )
                {
                    *p_out32++ = *p_in32++;
                }
                else
                {
                    uint32_t *p_pixel32 = p_line_end32 - ( p_in32 - p_line_start32 );
                    p_in32++;
                    *p_out++ = *p_pixel32;
                }
            }
            else
            {
                if ( b_left_to_right )
                {
                    uint32_t *p_pixel32 = p_line_end32 - ( p_in32 - p_line_start32 );
                    p_in32++;
                    *p_out++ = *p_pixel32;
                }
                else
                {
                    *p_out32++ = *p_in32++;
                }
            }
        }
        p_in = (uint8_t*) p_in32;
        p_out = (uint8_t*) p_out32;
        p_in += p_pic->p[i_plane].i_pitch - p_pic->p[i_plane].i_visible_pitch;
        p_out += p_outpic->p[i_plane].i_pitch
            - p_outpic->p[i_plane].i_visible_pitch;
    }
}
