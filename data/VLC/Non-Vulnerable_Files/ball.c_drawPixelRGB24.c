}
static void drawPixelRGB24( filter_sys_t *p_sys, picture_t *p_outpic,
                            uint8_t R, uint8_t G, uint8_t B,
                            int x, int y, bool b_skip )
{
    VLC_UNUSED( p_sys );
    VLC_UNUSED( b_skip );
    uint8_t *p_pixel = p_outpic->p[0].p_pixels
                       + p_outpic->p[0].i_pitch
                       * x + 3 * y;
    *p_pixel = B;
    *++p_pixel = G;
    *++p_pixel = R;
}
