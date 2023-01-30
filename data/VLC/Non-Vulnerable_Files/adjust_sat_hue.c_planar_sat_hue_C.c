}
int planar_sat_hue_C( picture_t * p_pic, picture_t * p_outpic, int i_sin, int i_cos,
                         int i_sat, int i_x, int i_y )
{
    uint8_t *p_in, *p_in_v, *p_in_end, *p_line_end;
    uint8_t *p_out, *p_out_v;
    p_in = p_pic->p[U_PLANE].p_pixels;
    p_in_v = p_pic->p[V_PLANE].p_pixels;
    p_in_end = p_in + p_pic->p[U_PLANE].i_visible_lines
                      * p_pic->p[U_PLANE].i_pitch - 8;
    p_out = p_outpic->p[U_PLANE].p_pixels;
    p_out_v = p_outpic->p[V_PLANE].p_pixels;
    uint8_t i_u, i_v;
    for( ; p_in < p_in_end ; )
    {
        p_line_end = p_in + p_pic->p[U_PLANE].i_visible_pitch - 8;
        for( ; p_in < p_line_end ; )
        {
            /* Do 8 pixels at a time */
            ADJUST_8_TIMES( PLANAR_WRITE_UV() );
        }
        p_line_end += 8;
        for( ; p_in < p_line_end ; )
        {
            PLANAR_WRITE_UV();
        }
        p_in += p_pic->p[U_PLANE].i_pitch
                - p_pic->p[U_PLANE].i_visible_pitch;
        p_in_v += p_pic->p[V_PLANE].i_pitch
                - p_pic->p[V_PLANE].i_visible_pitch;
        p_out += p_outpic->p[U_PLANE].i_pitch
                - p_outpic->p[U_PLANE].i_visible_pitch;
        p_out_v += p_outpic->p[V_PLANE].i_pitch
                    - p_outpic->p[V_PLANE].i_visible_pitch;
    }
    return VLC_SUCCESS;
}
