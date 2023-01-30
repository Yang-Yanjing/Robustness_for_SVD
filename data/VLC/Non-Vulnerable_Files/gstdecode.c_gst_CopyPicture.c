 * as possible, since it involves a complete frame copy. */
static void gst_CopyPicture( picture_t *p_pic, GstVideoFrame *p_frame )
{
    int i_plane, i_planes, i_line, i_dst_stride, i_src_stride;
    uint8_t *p_dst, *p_src;
    int i_w, i_h;
    i_planes = p_pic->i_planes;
    for( i_plane = 0; i_plane < i_planes; i_plane++ )
    {
        p_dst = p_pic->p[i_plane].p_pixels;
        p_src = GST_VIDEO_FRAME_PLANE_DATA( p_frame, i_plane );
        i_dst_stride = p_pic->p[i_plane].i_pitch;
        i_src_stride = GST_VIDEO_FRAME_PLANE_STRIDE( p_frame, i_plane );
        i_w = GST_VIDEO_FRAME_COMP_WIDTH( p_frame,
                i_plane ) * GST_VIDEO_FRAME_COMP_PSTRIDE( p_frame, i_plane );
        i_h = GST_VIDEO_FRAME_COMP_HEIGHT( p_frame, i_plane );
        for( i_line = 0;
                i_line < __MIN( p_pic->p[i_plane].i_lines, i_h );
                i_line++ )
        {
            memcpy( p_dst, p_src, i_w );
            p_src += i_src_stride;
            p_dst += i_dst_stride;
        }
    }
}
