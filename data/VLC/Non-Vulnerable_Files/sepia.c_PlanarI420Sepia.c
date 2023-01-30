 *****************************************************************************/
static void PlanarI420Sepia( picture_t *p_pic, picture_t *p_outpic,
                               int i_intensity )
{
#if defined(CAN_COMPILE_SSE2)
    if (vlc_CPU_SSE2())
        return PlanarI420SepiaSSE( p_pic, p_outpic, i_intensity );
#endif
    // prepared values to copy for U and V channels
    const uint8_t filling_const_8u = 128 - i_intensity / 6;
    const uint8_t filling_const_8v = 128 + i_intensity / 14;
    /* iterate for every two visible line in the frame */
    for( int y = 0; y < p_pic->p[Y_PLANE].i_visible_lines - 1; y += 2)
    {
        const int i_dy_line1_start = y * p_outpic->p[Y_PLANE].i_pitch;
        const int i_dy_line2_start = ( y + 1 ) * p_outpic->p[Y_PLANE].i_pitch;
        const int i_du_line_start = (y/2) * p_outpic->p[U_PLANE].i_pitch;
        const int i_dv_line_start = (y/2) * p_outpic->p[V_PLANE].i_pitch;
        // to prevent sigsegv if one pic is smaller (theoretically)
        int i_picture_size_limit = p_pic->p[Y_PLANE].i_visible_pitch
                  < p_outpic->p[Y_PLANE].i_visible_pitch
                  ? (p_pic->p[Y_PLANE].i_visible_pitch - 1) :
                  (p_outpic->p[Y_PLANE].i_visible_pitch - 1);
        /* iterate for every two visible line in the frame */
        for( int x = 0; x < i_picture_size_limit; x += 2)
        {
            // y = y - y/4 {to prevent overflow} + intensity / 4
            p_outpic->p[Y_PLANE].p_pixels[i_dy_line1_start + x] =
                p_pic->p[Y_PLANE].p_pixels[i_dy_line1_start + x] -
                (p_pic->p[Y_PLANE].p_pixels[i_dy_line1_start + x] >> 2) +
                (i_intensity >> 2);
            p_outpic->p[Y_PLANE].p_pixels[i_dy_line1_start + x + 1] =
                p_pic->p[Y_PLANE].p_pixels[i_dy_line1_start + x + 1] -
                (p_pic->p[Y_PLANE].p_pixels[i_dy_line1_start + x + 1] >> 2) +
                (i_intensity >> 2);
            p_outpic->p[Y_PLANE].p_pixels[i_dy_line2_start + x] =
                p_pic->p[Y_PLANE].p_pixels[i_dy_line2_start + x] -
                (p_pic->p[Y_PLANE].p_pixels[i_dy_line2_start + x] >> 2) +
                (i_intensity >> 2);
            p_outpic->p[Y_PLANE].p_pixels[i_dy_line2_start + x + 1] =
                p_pic->p[Y_PLANE].p_pixels[i_dy_line2_start + x + 1] -
                (p_pic->p[Y_PLANE].p_pixels[i_dy_line2_start + x + 1] >> 2) +
                (i_intensity >> 2);
            // u = 128 {half => B&W} - intensity / 6
            p_outpic->p[U_PLANE].p_pixels[i_du_line_start + (x / 2)] =
                filling_const_8u;
            // v = 128 {half => B&W} + intensity / 14
            p_outpic->p[V_PLANE].p_pixels[i_dv_line_start + (x / 2)] =
                filling_const_8v;
        }
    }
}
