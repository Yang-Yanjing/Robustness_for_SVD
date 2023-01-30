 *****************************************************************************/
void RenderMean( filter_t *p_filter,
                 picture_t *p_outpic, picture_t *p_pic )
{
    int i_plane;
    /* Copy image and skip lines */
    for( i_plane = 0 ; i_plane < p_pic->i_planes ; i_plane++ )
    {
        uint8_t *p_in, *p_out_end, *p_out;
        p_in = p_pic->p[i_plane].p_pixels;
        p_out = p_outpic->p[i_plane].p_pixels;
        p_out_end = p_out + p_outpic->p[i_plane].i_pitch
                             * p_outpic->p[i_plane].i_visible_lines;
        /* All lines: mean value */
        for( ; p_out < p_out_end ; )
        {
            Merge( p_out, p_in, p_in + p_pic->p[i_plane].i_pitch,
                   p_pic->p[i_plane].i_pitch );
            p_out += p_outpic->p[i_plane].i_pitch;
            p_in += 2 * p_pic->p[i_plane].i_pitch;
        }
    }
    EndMerge();
}
