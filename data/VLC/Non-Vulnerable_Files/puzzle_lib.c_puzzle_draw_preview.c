 *****************************************************************************/
void puzzle_draw_preview( filter_t *p_filter, picture_t *p_pic_in, picture_t *p_pic_out)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    for( uint8_t  i_plane = 0; i_plane < p_pic_out->i_planes; i_plane++ ) {
        int32_t i_preview_offset = 0;
        int32_t i_preview_width  = p_sys->ps_desk_planes[i_plane].i_width * p_sys->s_current_param.i_preview_size / 100;
        int32_t i_preview_lines  = p_pic_out->p[i_plane].i_visible_lines * p_sys->s_current_param.i_preview_size / 100;
        int32_t i_pixel_pitch    = p_pic_out->p[i_plane].i_pixel_pitch;
        const int32_t i_src_pitch  = p_pic_in->p[i_plane].i_pitch;
        const int32_t i_dst_pitch  = p_pic_out->p[i_plane].i_pitch;
        uint8_t *p_src = p_pic_in->p[i_plane].p_pixels;
        uint8_t *p_dst = p_pic_out->p[i_plane].p_pixels;
        switch ( p_sys->i_preview_pos ) {
        case 0:
            i_preview_offset = 0;
            break;
        case 1:
            i_preview_offset =
                (p_sys->ps_desk_planes[i_plane].i_width - 1 - i_preview_width) * i_pixel_pitch;
            break;
        case 2:
            i_preview_offset =
                (p_sys->ps_desk_planes[i_plane].i_width - 1 - i_preview_width) * i_pixel_pitch
                + ((int32_t) ( p_sys->ps_desk_planes[i_plane].i_lines - 1 - i_preview_lines )) * i_dst_pitch;
            break;
        case 3:
            i_preview_offset = ((int32_t) ( p_sys->ps_desk_planes[i_plane].i_lines - 1 - i_preview_lines )) * i_dst_pitch;
            break;
        default:
            i_preview_offset = 0;
            break;
        }
        for ( int32_t y = 0; y < i_preview_lines; y++ )
            for ( int32_t x = 0; x < i_preview_width; x++ )
                memcpy( &p_dst[ y * i_dst_pitch + x * i_pixel_pitch + i_preview_offset ],
                        &p_src[ ( y * 100 / p_sys->s_current_param.i_preview_size ) * i_src_pitch
                                + ( x * 100 / p_sys->s_current_param.i_preview_size ) * i_pixel_pitch ],
                        i_pixel_pitch );
    }
}
