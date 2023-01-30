 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* */
    switch( p_filter->fmt_in.i_codec )
    {
    CASE_PLANAR_YUV
    case VLC_CODEC_GREY:
        break;
    default:
        msg_Err( p_filter, "Unsupported chroma" );
        return VLC_EGENERIC;
    }
    if( !es_format_IsSimilar( &p_filter->fmt_in, &p_filter->fmt_out ) )
    {
        msg_Err( p_filter, "Input and output format does not match" );
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = p_sys = malloc( sizeof( *p_sys ) );
    if( !p_filter->p_sys )
        return VLC_ENOMEM;
    p_sys->p_image = image_HandlerCreate( p_filter );
    if( !p_sys->p_image )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_sys->i_x = 0;
    p_sys->i_y = 0;
    p_sys->i_zoom = 2*ZOOM_FACTOR;
    p_sys->b_visible = true;
    p_sys->i_last_activity = mdate();
    p_sys->i_hide_timeout = 1000 * var_CreateGetInteger( p_filter, "mouse-hide-timeout" ); /* FIXME */
    /* */
    p_filter->pf_video_filter = Filter;
    p_filter->pf_video_mouse = Mouse;
    return VLC_SUCCESS;
}
