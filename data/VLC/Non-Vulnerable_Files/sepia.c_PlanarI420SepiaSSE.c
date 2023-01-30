VLC_SSE
static void PlanarI420SepiaSSE( picture_t *p_pic, picture_t *p_outpic,
                                int i_intensity )
{
    /* prepared values to copy for U and V channels */
    const uint8_t filling_const_8u = 128 - i_intensity / 6;
    const uint8_t filling_const_8v = 128 + i_intensity / 14;
    /* prepared value for faster broadcasting in xmm register */
    int i_intensity_spread = 0x10001 * (uint8_t) i_intensity;
    __asm__ volatile(
        "pxor      %%xmm7, %%xmm7\n"
        ::: "xmm7");
    /* iterate for every two visible line in the frame */
    for (int y = 0; y < p_pic->p[Y_PLANE].i_visible_lines - 1; y += 2)
    {
        const int i_dy_line1_start = y * p_outpic->p[Y_PLANE].i_pitch;
        const int i_dy_line2_start = (y + 1) * p_outpic->p[Y_PLANE].i_pitch;
        const int i_du_line_start =  (y / 2) * p_outpic->p[U_PLANE].i_pitch;
        const int i_dv_line_start =  (y / 2) * p_outpic->p[V_PLANE].i_pitch;
        int x = 0;
        /* iterate for every visible line in the frame (eight values at once) */
        for ( ; x < p_pic->p[Y_PLANE].i_visible_pitch - 15; x += 16 )
        {
            /* Compute yellow channel values with asm function */
            Sepia8ySSE2(&p_outpic->p[Y_PLANE].p_pixels[i_dy_line1_start + x],
                        &p_pic->p[Y_PLANE].p_pixels[i_dy_line1_start + x],
                        i_intensity_spread );
            Sepia8ySSE2(&p_outpic->p[Y_PLANE].p_pixels[i_dy_line2_start + x],
                        &p_pic->p[Y_PLANE].p_pixels[i_dy_line2_start + x],
                        i_intensity_spread );
            Sepia8ySSE2(&p_outpic->p[Y_PLANE].p_pixels[i_dy_line1_start + x + 8],
                        &p_pic->p[Y_PLANE].p_pixels[i_dy_line1_start + x + 8],
                        i_intensity_spread );
            Sepia8ySSE2(&p_outpic->p[Y_PLANE].p_pixels[i_dy_line2_start + x + 8],
                        &p_pic->p[Y_PLANE].p_pixels[i_dy_line2_start + x + 8],
                        i_intensity_spread );
            /* Copy precomputed values to destination memory location */
            memset(&p_outpic->p[U_PLANE].p_pixels[i_du_line_start + (x / 2)],
                   filling_const_8u, 8 );
            memset(&p_outpic->p[V_PLANE].p_pixels[i_dv_line_start + (x / 2)],
                   filling_const_8v, 8 );
        }
        /* Completing the job, the cycle above takes really big chunks, so
           this makes sure the job will be done completely */
        for ( ; x < p_pic->p[Y_PLANE].i_visible_pitch - 1; x += 2 )
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
