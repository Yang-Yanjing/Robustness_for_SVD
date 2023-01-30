 */
static int oldmovie_dark_border_effect( filter_t *p_filter, picture_t *p_pic_out )
{
    filter_sys_t *p_sys = p_filter->p_sys;
#define BORDER_DIST 20
    for ( int32_t i_y = 0; i_y < p_sys->i_height[Y_PLANE]; i_y++ )
        for ( int32_t i_x = 0; i_x < p_sys->i_width[Y_PLANE]; i_x++ ) {
            int32_t i_x_border_dist = __MIN( i_x, p_sys->i_width[Y_PLANE] - i_x);
            int32_t i_y_border_dist = __MIN( i_y, p_sys->i_height[Y_PLANE] - i_y);
            int32_t i_border_dist = __MAX(BORDER_DIST - i_x_border_dist,0)
                                  + __MAX(BORDER_DIST - i_y_border_dist,0);
            i_border_dist = __MIN(BORDER_DIST, i_border_dist);
            if ( i_border_dist == 0 )
                continue;
            uint32_t i_pix_ofs = i_x * p_pic_out->p[Y_PLANE].i_pixel_pitch
                               + i_y * p_pic_out->p[Y_PLANE].i_pitch;
            SUB_MIN( p_pic_out->p[Y_PLANE].p_pixels[i_pix_ofs],
                     i_border_dist * 255 / BORDER_DIST, 0 );
        }
    return VLC_SUCCESS;
}
