 *****************************************************************************/
void RenderLinear( filter_t *p_filter,
                   picture_t *p_outpic, picture_t *p_pic, int i_field )
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
        /* For BOTTOM field we need to add the first line */
        if( i_field == 1 )
        {
            memcpy( p_out, p_in, p_pic->p[i_plane].i_pitch );
            p_in += p_pic->p[i_plane].i_pitch;
            p_out += p_outpic->p[i_plane].i_pitch;
        }
        p_out_end -= 2 * p_outpic->p[i_plane].i_pitch;
        for( ; p_out < p_out_end ; )
        {
            memcpy( p_out, p_in, p_pic->p[i_plane].i_pitch );
            p_out += p_outpic->p[i_plane].i_pitch;
            Merge( p_out, p_in, p_in + 2 * p_pic->p[i_plane].i_pitch,
                   p_pic->p[i_plane].i_pitch );
            p_in += 2 * p_pic->p[i_plane].i_pitch;
            p_out += p_outpic->p[i_plane].i_pitch;
        }
        memcpy( p_out, p_in, p_pic->p[i_plane].i_pitch );
        /* For TOP field we need to add the last line */
        if( i_field == 0 )
        {
            p_in += p_pic->p[i_plane].i_pitch;
            p_out += p_outpic->p[i_plane].i_pitch;
            memcpy( p_out, p_in, p_pic->p[i_plane].i_pitch );
        }
    }
    EndMerge();
}
