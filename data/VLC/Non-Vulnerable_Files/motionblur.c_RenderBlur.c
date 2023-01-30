 *****************************************************************************/
static void RenderBlur( filter_sys_t *p_sys, picture_t *p_newpic,
                        picture_t *p_outpic )
{
    int i_plane;
    const int i_oldfactor = atomic_load( &p_sys->i_factor );
    int i_newfactor = 128 - i_oldfactor;
    for( i_plane = 0; i_plane < p_outpic->i_planes; i_plane++ )
    {
        uint8_t *p_old, *p_new, *p_out, *p_out_end, *p_out_line_end;
        const int i_visible_pitch = p_outpic->p[i_plane].i_visible_pitch;
        const int i_visible_lines = p_outpic->p[i_plane].i_visible_lines;
        p_out = p_outpic->p[i_plane].p_pixels;
        p_new = p_newpic->p[i_plane].p_pixels;
        p_old = p_sys->p_tmp->p[i_plane].p_pixels;
        p_out_end = p_out + p_outpic->p[i_plane].i_pitch * i_visible_lines;
        while ( p_out < p_out_end )
        {
            p_out_line_end = p_out + i_visible_pitch;
            while ( p_out < p_out_line_end )
            {
                *p_out++ = (((*p_old++) * i_oldfactor) +
                            ((*p_new++) * i_newfactor)) >> 7;
            }
            p_old += p_sys->p_tmp->p[i_plane].i_pitch - i_visible_pitch;
            p_new += p_newpic->p[i_plane].i_pitch     - i_visible_pitch;
            p_out += p_outpic->p[i_plane].i_pitch     - i_visible_pitch;
        }
    }
}
