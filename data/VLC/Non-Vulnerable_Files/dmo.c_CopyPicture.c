}
static void CopyPicture( picture_t *p_pic, uint8_t *p_in )
{
    int i_plane, i_line, i_width, i_dst_stride;
    uint8_t *p_dst, *p_src = p_in;
    p_dst = p_pic->p[1].p_pixels;
    p_pic->p[1].p_pixels = p_pic->p[2].p_pixels;
    p_pic->p[2].p_pixels = p_dst;
    for( i_plane = 0; i_plane < p_pic->i_planes; i_plane++ )
    {
        p_dst = p_pic->p[i_plane].p_pixels;
        i_width = p_pic->p[i_plane].i_visible_pitch;
        i_dst_stride  = p_pic->p[i_plane].i_pitch;
        for( i_line = 0; i_line < p_pic->p[i_plane].i_visible_lines; i_line++ )
        {
            memcpy( p_dst, p_src, i_width );
            p_src += i_width;
            p_dst += i_dst_stride;
        }
    }
    p_dst = p_pic->p[1].p_pixels;
    p_pic->p[1].p_pixels = p_pic->p[2].p_pixels;
    p_pic->p[2].p_pixels = p_dst;
}
