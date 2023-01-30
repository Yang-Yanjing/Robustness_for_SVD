 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    const video_format_t *p_fmt = &p_filter->fmt_in.video;
    filter_sys_t *p_sys;
    bool is_yuv_planar;
    switch( p_fmt->i_chroma )
    {
        CASE_PLANAR_YUV
            is_yuv_planar = true;
            break;
        CASE_PACKED_YUV_422
            is_yuv_planar = false;
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                     (char*)&(p_fmt->i_chroma) );
            return VLC_EGENERIC;
    }
    p_filter->pf_video_filter = Filter;
    /* Allocate structure */
    p_filter->p_sys = p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->is_yuv_planar = is_yuv_planar;
    p_sys->b_old = false;
    p_sys->p_old = picture_NewFromFormat( p_fmt );
    p_sys->p_buf  = calloc( p_fmt->i_width * p_fmt->i_height, sizeof(*p_sys->p_buf) );
    p_sys->p_buf2 = calloc( p_fmt->i_width * p_fmt->i_height, sizeof(*p_sys->p_buf) );
    if( !p_sys->p_old || !p_sys->p_buf || !p_sys->p_buf2 )
    {
        free( p_sys->p_buf2 );
        free( p_sys->p_buf );
        if( p_sys->p_old )
            picture_Release( p_sys->p_old );
        return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}
