 *****************************************************************************/
static picture_t *PostprocPict( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const uint8_t *src[3];
    uint8_t *dst[3];
    int i_plane;
    int i_src_stride[3], i_dst_stride[3];
    picture_t *p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* Lock to prevent issues if pp_mode is changed */
    vlc_mutex_lock( &p_sys->lock );
    if( !p_sys->pp_mode )
    {
        vlc_mutex_unlock( &p_sys->lock );
        picture_CopyPixels( p_outpic, p_pic );
        return CopyInfoAndRelease( p_outpic, p_pic );
    }
    for( i_plane = 0; i_plane < p_pic->i_planes; i_plane++ )
    {
        src[i_plane] = p_pic->p[i_plane].p_pixels;
        dst[i_plane] = p_outpic->p[i_plane].p_pixels;
        /* I'm not sure what happens if i_pitch != i_visible_pitch ...
         * at least it shouldn't crash. */
        i_src_stride[i_plane] = p_pic->p[i_plane].i_pitch;
        i_dst_stride[i_plane] = p_outpic->p[i_plane].i_pitch;
    }
    pp_postprocess( src, i_src_stride, dst, i_dst_stride,
                    p_filter->fmt_in.video.i_width,
                    p_filter->fmt_in.video.i_height, NULL, 0,
                    p_sys->pp_mode, p_sys->pp_context, 0 );
    vlc_mutex_unlock( &p_sys->lock );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
