 *****************************************************************************/
static void RVPosterize( picture_t *p_pic, picture_t *p_outpic,
                                 bool rv32, int level )
{
    uint8_t *p_in, *p_in_end, *p_line_end, *p_out, pixel;
    p_in = p_pic->p[0].p_pixels;
    p_in_end = p_in + p_pic->p[0].i_visible_lines
        * p_pic->p[0].i_pitch;
    p_out = p_outpic->p[0].p_pixels;
    while( p_in < p_in_end )
    {
        p_line_end = p_in + p_pic->p[0].i_visible_pitch;
        while( p_in < p_line_end )
        {
            pixel = *p_in++;
            *p_out++ = POSTERIZE_PIXEL( pixel, level );
            pixel = *p_in++;
            *p_out++ = POSTERIZE_PIXEL( pixel, level );
            pixel = *p_in++;
            *p_out++ = POSTERIZE_PIXEL( pixel, level );
            /* for rv32 we take 4 chunks at the time */
            if ( rv32 )
            {
                pixel = *p_in++;
                *p_out++ = POSTERIZE_PIXEL( pixel, level );
            }
        }
        p_in += p_pic->p[0].i_pitch - p_pic->p[0].i_visible_pitch;
        p_out += p_outpic->p[0].i_pitch
            - p_outpic->p[0].i_visible_pitch;
    }
}
