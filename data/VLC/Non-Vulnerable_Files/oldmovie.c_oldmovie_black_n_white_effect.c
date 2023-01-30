 */
static void oldmovie_black_n_white_effect( picture_t *p_pic_out )
{
    for ( int32_t i_y = 0; i_y < p_pic_out->p[Y_PLANE].i_visible_lines; i_y++ )
        for ( int32_t i_x = 0; i_x < p_pic_out->p[Y_PLANE].i_visible_pitch;
              i_x += p_pic_out->p[Y_PLANE].i_pixel_pitch ) {
            uint32_t i_pix_ofs = i_x+i_y*p_pic_out->p[Y_PLANE].i_pitch;
            p_pic_out->p[Y_PLANE].p_pixels[i_pix_ofs] -= p_pic_out->p[Y_PLANE].p_pixels[i_pix_ofs] >> 2;
            p_pic_out->p[Y_PLANE].p_pixels[i_pix_ofs] += 30;
        }
    memset( p_pic_out->p[U_PLANE].p_pixels, 122,
            p_pic_out->p[U_PLANE].i_lines * p_pic_out->p[U_PLANE].i_pitch );
    memset( p_pic_out->p[V_PLANE].p_pixels, 132,
            p_pic_out->p[V_PLANE].i_lines * p_pic_out->p[V_PLANE].i_pitch );
}
