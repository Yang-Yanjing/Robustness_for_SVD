}
static void drawPixelPacked( filter_sys_t *p_sys, picture_t *p_outpic,
                           uint8_t Y, uint8_t U, uint8_t V,
                           int x, int y, bool b_skip )
{
    uint8_t *p_pixel = p_outpic->p[0].p_pixels
                       + p_outpic->p[0].i_pitch * y + x * 2;
    *( p_pixel + p_sys->i_y_offset ) = Y;
    if( !b_skip )
    {
        *( p_pixel + p_sys->i_u_offset ) = U;
        *( p_pixel + p_sys->i_v_offset ) = V;
    }
}
