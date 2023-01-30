 *****************************************************************************/
void puzzle_preset_desk_background( picture_t *p_pic_out, uint8_t Y, uint8_t U, uint8_t V)
{
    uint8_t i_c;
    for( uint8_t i_plane = 0; i_plane < p_pic_out->i_planes; i_plane++ ) {
        if (i_plane == Y_PLANE)
            i_c = Y;
        else if (i_plane == U_PLANE)
            i_c = U;
        else if (i_plane == V_PLANE)
            i_c = V;
        const int32_t i_dst_pitch = p_pic_out->p[i_plane].i_pitch;
        const int32_t i_dst_lines = p_pic_out->p[i_plane].i_lines;
        uint8_t *p_dst = p_pic_out->p[i_plane].p_pixels;
        for (int32_t y = 0; y < i_dst_lines; y++)
            memset(&p_dst[y * i_dst_pitch], i_c, i_dst_pitch);
    }
}
