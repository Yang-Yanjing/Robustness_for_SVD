 *****************************************************************************/
static void I422_YV12( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    uint16_t i_dpy = p_dest->p[Y_PLANE].i_pitch;
    uint16_t i_spy = p_source->p[Y_PLANE].i_pitch;
    uint16_t i_dpuv = p_dest->p[U_PLANE].i_pitch;
    uint16_t i_spuv = p_source->p[U_PLANE].i_pitch;
    uint16_t i_width = p_filter->fmt_in.video.i_width;
    uint16_t i_y = p_filter->fmt_in.video.i_height;
    uint8_t *p_dy = p_dest->Y_PIXELS + (i_y-1)*i_dpy;
    uint8_t *p_y = p_source->Y_PIXELS + (i_y-1)*i_spy;
    uint8_t *p_du = p_dest->V_PIXELS + (i_y/2-1)*i_dpuv; /* U and V are swapped */
    uint8_t *p_u = p_source->U_PIXELS + (i_y-1)*i_spuv;
    uint8_t *p_dv = p_dest->U_PIXELS + (i_y/2-1)*i_dpuv; /* U and V are swapped */
    uint8_t *p_v = p_source->V_PIXELS + (i_y-1)*i_spuv;
    i_y /= 2;
    for ( ; i_y--; )
    {
        memcpy(p_dy, p_y, i_width); p_dy -= i_dpy; p_y -= i_spy;
        memcpy(p_dy, p_y, i_width); p_dy -= i_dpy; p_y -= i_spy;
        memcpy(p_du, p_u, i_width/2); p_du -= i_dpuv; p_u -= 2*i_spuv;
        memcpy(p_dv, p_v, i_width/2); p_dv -= i_dpuv; p_v -= 2*i_spuv;
    }
}
